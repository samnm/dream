#include "shaders.h"

const char * DREAM_FRAG = {
  "#version 150\n"
  "out vec4 outColor;\n"
  "void main()\n"
  "{\n"
  "    outColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
  "}\n"
};

const char * DREAM_VERT = {
  "#version 150\n"
  "in vec2 position;\n"
  "void main()\n"
  "{\n"
  "    gl_Position = vec4(position, 0.0, 1.0);\n"
  "}\n"
};

