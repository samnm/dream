#include "shaders.h"

const char * DREAM_FRAG = {
  "#version 150\n"
  "in vec3 fragVert;\n"
  "in vec3 fragNormal;\n"
  "in vec3 fragColor;\n"
  "uniform mat4 model;\n"
  "uniform vec3 lightPos;\n"
  "out vec4 outColor;\n"
  "void main()\n"
  "{\n"
  "  //calculate normal in world coordinates\n"
  "  // mat3 normalMatrix = transpose(inverse(mat3(model)));\n"
  "  vec3 normal = normalize(model * vec4(fragNormal, 0)).xyz;\n"
  "  //calculate the location of this fragment (pixel) in world coordinates\n"
  "  vec3 fragPosition = vec3(model * vec4(fragVert, 1));\n"
  "  //calculate the vector from this pixels surface to the light source\n"
  "  vec3 surfaceToLight = lightPos - fragPosition;\n"
  "  //calculate the cosine of the angle of incidence\n"
  "  // dividing by surfaceToLight dims light the further from the light source we go\n"
  "  float brightness = dot(normal, surfaceToLight) / (length(surfaceToLight));\n"
  "  brightness = clamp(brightness, 0, 1) * 0.8 + 0.2;\n"
  "  outColor = vec4(brightness * fragColor, 1.0);\n"
  "}\n"
};

const char * DREAM_GEOM = {
  "#version 150\n"
  "layout(points) in;\n"
  "layout(triangle_strip, max_vertices = 4) out;\n"
  "in vec3 geomVert[];\n"
  "in vec3 geomNormal[];\n"
  "in vec3 geomScreenNormal[];\n"
  "in vec3 geomColor[];\n"
  "out vec3 fragVert;\n"
  "out vec3 fragNormal;\n"
  "out vec3 fragColor;\n"
  "void main()\n"
  "{\n"
  "  fragVert = geomVert[0];\n"
  "  fragNormal = geomNormal[0];\n"
  "  fragColor = geomColor[0];\n"
  "  float d = 0.025;\n"
  "  gl_Position = gl_in[0].gl_Position + vec4(-d, -d, 0.0f, 0.0f);    // 1:bottom-left\n"
  "  EmitVertex();\n"
  "  gl_Position = gl_in[0].gl_Position + vec4( d, -d, 0.0f, 0.0f);    // 2:bottom-right\n"
  "  EmitVertex();\n"
  "  gl_Position = gl_in[0].gl_Position + vec4(-d,  d, 0.0f, 0.0f);    // 3:top-left\n"
  "  EmitVertex();\n"
  "  gl_Position = gl_in[0].gl_Position + vec4( d,  d, 0.0f, 0.0f);    // 4:top-right\n"
  "  EmitVertex();\n"
  "  EndPrimitive();\n"
  "}\n"
};

const char * DREAM_VERT = {
  "#version 150\n"
  "in vec3 position;\n"
  "in vec3 normal;\n"
  "in vec3 color;\n"
  "uniform mat4 model;\n"
  "uniform mat4 view;\n"
  "uniform mat4 proj;\n"
  "out vec3 geomVert;\n"
  "out vec3 geomNormal;\n"
  "out vec3 geomColor;\n"
  "void main()\n"
  "{\n"
  "  geomVert = position;\n"
  "  geomNormal = normal;\n"
  "  geomColor = color;\n"
  "  gl_Position = proj * view * model * vec4(position, 1.0);\n"
  "}\n"
};

