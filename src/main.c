#include <GLFW/glfw3.h>

#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "linmath.h"
#include "octtree.h"
#include "point.h"
#include "sdf.h"
#include "shaders.h"

static void error_callback(int error, const char* description)
{
  fputs(description, stderr);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
}

GLuint loadShader(GLenum type, const GLchar* src)
{
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &src, NULL);
  glCompileShader(shader);
  return shader;
}

struct {
  Primative **edits;
  int num_edits;
} sdf_args;

float distance(vec4 point)
{
  assert(sdf_args.num_edits <= 0 || sdf_args.edits != NULL);

  float dist = FLT_MAX;
  for (int i = 0; i < sdf_args.num_edits; ++i)
  {
    Primative *primative = sdf_args.edits[i];
    switch (primative->operation)
    {
      case ADDITIVE:
        dist = primative->blend > 0
          ? smin(dist, primative_distance(primative, point), primative->blend)
          : min(dist, primative_distance(primative, point));
        break;

      case SUBTRACTIVE:
        dist = max(dist, -primative_distance(primative, point));
        break;
    }
  }

  return dist;
}

int main(void)
{
  Primative *edits[] = {
    primative_create(SPHERE, ADDITIVE),
    primative_create(SPHERE, ADDITIVE)
  };

  mat4x4_scale_aniso(edits[0]->transformation, edits[0]->transformation, 2.0, 1.0, 0.75);
  edits[1]->blend = 0.5;

  sdf_args.edits = edits;
  sdf_args.num_edits = 2;

  vec3 origin = {0, 0, 0};
  vec3 halfDim = {1, 1, 1};
  OctTree *tree = octTree_create(origin, halfDim);

  octTree_populate(tree, distance);

  int num_verticies = octTree_count(tree);
  size_t vert_size = sizeof(Point);

  int vi = 0;
  Point *verticies = malloc(num_verticies * vert_size);
  octTree_get_points(tree, verticies, &vi);

  GLFWwindow* window;

  mat4x4 ident;
  mat4x4_identity(ident);

  glfwSetErrorCallback(error_callback);

  if (!glfwInit())
    exit(EXIT_FAILURE);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  window = glfwCreateWindow(640, 480, "speed dream", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  glfwSetKeyCallback(window, key_callback);

  mat4x4 proj;
  mat4x4 view;
  mat4x4 matrix;

  mat4x4_identity(matrix);
  mat4x4_rotate(matrix, ident, 0.0f, 0.0f, 1.0f, 3.141/4);

  vec3 eye = {1.2f, 1.2f, 1.2f};
  vec3 center = {0.0f, 0.0f, 0.0f};
  vec3 up = {0.0f, 0.0f, 1.0f};
  mat4x4_identity(view);
  mat4x4_look_at(view, eye, center, up);

  // Create Vertex Array Object
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Create a Vertex Buffer Object and copy the vertex data to it
  GLuint vbo;
  glGenBuffers(1, &vbo);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, num_verticies * vert_size, verticies, GL_STATIC_DRAW);

  // Create and compile the shaders
  GLuint vertexShader = loadShader(GL_VERTEX_SHADER, DREAM_VERT);
  GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, DREAM_FRAG);

  // Link the vertex and fragment shader into a shader program
  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glBindFragDataLocation(shaderProgram, 0, "outColor");
  glLinkProgram(shaderProgram);
  glUseProgram(shaderProgram);

  // Specify the layout of the vertex data
  GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
  glEnableVertexAttribArray(posAttrib);
  glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, vert_size, 0);

  GLint normAttrib = glGetAttribLocation(shaderProgram, "normal");
  glEnableVertexAttribArray(normAttrib);
  glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, vert_size, (void*)(3 * sizeof(GLfloat)));

  GLint colorAttrib = glGetAttribLocation(shaderProgram, "color");
  glEnableVertexAttribArray(colorAttrib);
  glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, vert_size, (void*)(6 * sizeof(GLfloat)));

  GLint uniModel = glGetUniformLocation(shaderProgram, "model");
  GLint uniView = glGetUniformLocation(shaderProgram, "view");
  GLint uniProj = glGetUniformLocation(shaderProgram, "proj");

  glUniformMatrix4fv(uniView, 1, GL_FALSE, (const GLfloat *)&view);

  vec3 lightPos = {3.f, 0.f, 3.f};
  GLint uniLightPos = glGetUniformLocation(shaderProgram, "lightPos");
  glUniform3fv(uniLightPos, 1, (const GLfloat *)&lightPos);

  while (!glfwWindowShouldClose(window))
  {
    float ratio;
    int width, height;

    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float) height;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    mat4x4_perspective(proj, 60 * M_PI / 180, ratio, 0.1f, 10.0f);
    glUniformMatrix4fv(uniProj, 1, GL_FALSE, (const GLfloat *)&proj);

    mat4x4_rotate(matrix, ident, 0.0f, 0.0f, 1.0f, glfwGetTime() * 3.141);
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, (const GLfloat *)&matrix);

    glDrawArrays(GL_POINTS, 0, num_verticies);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteProgram(shaderProgram);
  glDeleteShader(fragmentShader);
  glDeleteShader(vertexShader);

  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);

  glfwDestroyWindow(window);

  free(verticies);

  glfwTerminate();
  exit(EXIT_SUCCESS);
}
