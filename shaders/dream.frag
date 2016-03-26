#version 150

in vec3 fragVert;
in vec3 fragNormal;
in vec3 fragColor;

uniform mat4 model;
uniform vec3 lightPos;

out vec4 outColor;

void main()
{
  //calculate normal in world coordinates
  // mat3 normalMatrix = transpose(inverse(mat3(model)));
  vec3 normal = normalize(model * vec4(fragNormal, 0)).xyz;

  //calculate the location of this fragment (pixel) in world coordinates
  vec3 fragPosition = vec3(model * vec4(fragVert, 1));

  //calculate the vector from this pixels surface to the light source
  vec3 surfaceToLight = lightPos - fragPosition;

  //calculate the cosine of the angle of incidence
  // dividing by surfaceToLight dims light the further from the light source we go
  float brightness = dot(normal, surfaceToLight) / (length(surfaceToLight));
  brightness = clamp(brightness, 0, 1);

  outColor = vec4(brightness * fragColor, 1.0);
}
