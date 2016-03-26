#version 150

in vec3 position;
in vec3 normal;
in vec3 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 geomVert;
out vec3 geomNormal;
out vec3 geomColor;

void main()
{
  geomVert = position;
  geomNormal = normal;
  geomColor = color;

  gl_Position = proj * view * model * vec4(position, 1.0);
}
