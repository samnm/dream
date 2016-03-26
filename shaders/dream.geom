#version 150

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec3 geomVert[];
in vec3 geomNormal[];
in vec3 geomScreenNormal[];
in vec3 geomColor[];

out vec3 fragVert;
out vec3 fragNormal;
out vec3 fragColor;

void main()
{
  fragVert = geomVert[0];
  fragNormal = geomNormal[0];
  fragColor = geomColor[0];

  float d = 0.025;
  gl_Position = gl_in[0].gl_Position + vec4(-d, -d, 0.0f, 0.0f);    // 1:bottom-left
  EmitVertex();
  gl_Position = gl_in[0].gl_Position + vec4( d, -d, 0.0f, 0.0f);    // 2:bottom-right
  EmitVertex();
  gl_Position = gl_in[0].gl_Position + vec4(-d,  d, 0.0f, 0.0f);    // 3:top-left
  EmitVertex();
  gl_Position = gl_in[0].gl_Position + vec4( d,  d, 0.0f, 0.0f);    // 4:top-right
  EmitVertex();

  EndPrimitive();
}
