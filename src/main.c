#include <GLFW/glfw3.h>

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "linmath.h"
#include "octtree.h"
#include "point.h"
#include "shaders.h"

typedef struct {
  vec3 pos;
  vec3 rot;
  vec3 scale;
  vec3 params;

  float blend;
  bool isSubtractive;
} Primative;

static void error_callback(int error, const char* description)
{
  fputs(description, stderr);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
}

int main(void)
{
  GLFWwindow* window;

  mat4x4 ident;
  mat4x4_identity(ident);

  vec3 origin = {0, 0, 0};
  vec3 halfDim = {1, 1, 1};
  Point *point = point_create();
  OctTree *tree = octTree_create(origin, halfDim);
  octTree_insert(tree, point);

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

  float r = 1.0;
  float ang = 2*M_PI/3;
  GLfloat vertices[] = {
      r * sinf(0      ), r * cosf(0      ),
      r * sinf(1 * ang), r * cosf(1 * ang),
      r * sinf(2 * ang), r * cosf(2 * ang)
  };

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
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Create and compile the vertex shader
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &DREAM_VERT, NULL);
  glCompileShader(vertexShader);

  // Create and compile the fragment shader
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &DREAM_FRAG, NULL);
  glCompileShader(fragmentShader);

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
  glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

  GLint uniModel = glGetUniformLocation(shaderProgram, "model");
  GLint uniView = glGetUniformLocation(shaderProgram, "view");
  GLint uniProj = glGetUniformLocation(shaderProgram, "proj");

  glUniformMatrix4fv(uniView, 1, GL_FALSE, (const GLfloat *)&view);

  while (!glfwWindowShouldClose(window))
  {
    float ratio;
    int width, height;

    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float) height;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    mat4x4_perspective(proj, 60 * M_PI / 180, width / height, 1.0f, 10.0f);
    glUniformMatrix4fv(uniProj, 1, GL_FALSE, (const GLfloat *)&proj);

    mat4x4_rotate(matrix, ident, 0.0f, 0.0f, 1.0f, glfwGetTime() * 3.141);
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, (const GLfloat *)&matrix);

    glDrawArrays(GL_POINTS, 0, 3);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteProgram(shaderProgram);
  glDeleteShader(fragmentShader);
  glDeleteShader(vertexShader);

  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);

  glfwDestroyWindow(window);

  glfwTerminate();
  exit(EXIT_SUCCESS);
}
