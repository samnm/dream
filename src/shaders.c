#include "shaders.h"

const char * DREAM_FRAG = {
  "#version 150\n"
  "in vec3 inColor;\n"
  "out vec4 outColor;\n"
  "void main()\n"
  "{\n"
  "    outColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
  "}\n"
};

const char * DREAM_VERT = {
  "#version 150\n"
  "in vec3 position;\n"
  "in vec3 normal;\n"
  "uniform mat4 model;\n"
  "uniform mat4 view;\n"
  "uniform mat4 proj;\n"
  "void main()\n"
  "{\n"
  "    gl_Position = proj * view * model * vec4(position, 1.0);\n"
  "}\n"
};

