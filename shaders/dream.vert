#version 150

in vec3 position;
in vec3 normal;
in vec3 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 fragVert;
out vec3 fragNormal;
out vec3 fragColor;

void main()
{
  fragVert = position;
  fragNormal = normal;
  fragColor = color;

  gl_Position = proj * view * model * vec4(position, 1.0);
  gl_PointSize = 10.0;
}
