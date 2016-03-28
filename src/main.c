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

  /*GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  char buffer[512];
  glGetShaderInfoLog(shader, 512, NULL, buffer);
  fprintf(stdout, "compiled shader\n");
  fprintf(stdout, "%s\n", buffer);*/

  return shader;
}

struct {
  Primative **edits;
  int num_edits;
} sdf_args = {0};

struct {
  vec3 pos;
  float pitch;
  float yaw;
} camera = {0};

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
    primative_create(CUBE, ADDITIVE),
    primative_create(SPHERE, ADDITIVE),
    primative_create(SPHERE, SUBTRACTIVE)
  };

  primative_scale(edits[0], 0.4, 1.0, 1.5);
  primative_translate(edits[1], 0.2, 0, 0);
  primative_translate(edits[2], -0.3, 0, 0);
  edits[1]->blend = 0.2;

  sdf_args.edits = edits;
  sdf_args.num_edits = 3;

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

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  glfwSetKeyCallback(window, key_callback);

  mat4x4 proj;
  mat4x4 view;
  mat4x4 matrix;

  mat4x4_identity(view);
  mat4x4_identity(matrix);

  vec3_set(camera.pos, 0.0f, 0.0f, 0.0f);

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
  GLuint geometryShader = loadShader(GL_GEOMETRY_SHADER, DREAM_GEOM);
  GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, DREAM_FRAG);

  // Link the vertex and fragment shader into a shader program
  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, geometryShader);
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    vec4 movement = {0, 0, 0, 1};
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
      movement[0] -= 0.1;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
      movement[0] += 0.1;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
      movement[2] += 0.1;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
      movement[2] -= 0.1;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
      camera.pitch -= 0.05;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
      camera.pitch += 0.05;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
      camera.yaw += 0.05;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
      camera.yaw -= 0.05;

    mat4x4 pitch_mat;
    mat4x4_identity(pitch_mat);
    mat4x4_rotate_X(pitch_mat, pitch_mat, camera.pitch);

    mat4x4 yaw_mat;
    mat4x4_identity(yaw_mat);
    mat4x4_rotate_Y(yaw_mat, yaw_mat, camera.yaw);

    mat4x4 inv_yaw_mat;
    mat4x4_invert(inv_yaw_mat, yaw_mat);

    vec4 rotated_movement;
    mat4x4_mul_vec4(rotated_movement, inv_yaw_mat, movement);
    camera.pos[0] += rotated_movement[0];
    camera.pos[1] += rotated_movement[1];
    camera.pos[2] += rotated_movement[2];

    mat4x4 translation_mat;
    mat4x4_translate(translation_mat, camera.pos[0], camera.pos[1], camera.pos[2]);

    mat4x4_mul(view, pitch_mat, yaw_mat);
    mat4x4_mul(view, view, translation_mat);
    glUniformMatrix4fv(uniView, 1, GL_FALSE, (const GLfloat *)&view);

    mat4x4_perspective(proj, 60 * M_PI / 180, ratio, 0.1f, 10.0f);
    glUniformMatrix4fv(uniProj, 1, GL_FALSE, (const GLfloat *)&proj);

    glUniformMatrix4fv(uniModel, 1, GL_FALSE, (const GLfloat *)&matrix);

    glDrawArrays(GL_POINTS, 0, num_verticies);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteProgram(shaderProgram);
  glDeleteShader(fragmentShader);
  glDeleteShader(geometryShader);
  glDeleteShader(vertexShader);

  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);

  glfwDestroyWindow(window);

  free(verticies);

  glfwTerminate();
  exit(EXIT_SUCCESS);
}
