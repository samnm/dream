#version 150

layout(points) in;
layout(points, max_vertices = 1) out;

in vec3 geomVert[];
in vec3 geomNormal[];
in vec3 geomColor[];

out vec3 fragVert;
out vec3 fragNormal;
out vec3 fragColor;

void main()
{
  gl_Position = gl_in[0].gl_Position;
  fragVert = geomVert[0];
  fragNormal = geomNormal[0];
  fragColor = geomColor[0];

  EmitVertex();
  EndPrimitive();
}
