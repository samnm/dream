#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "point.h"
#include "sdf.h"

Primative *primative_create(csg_shape shape, csg_operation operation)
{
  Primative *primative = malloc(sizeof(Primative));
  assert(primative != NULL);

  mat4x4_identity(primative->transformation);
  primative->blend = 0;
  primative->shape = shape;
  primative->operation = operation;

  return primative;
}

void primative_destroy(Primative *primative)
{
  assert(primative != NULL);
  free(primative);
}

void primative_translate(Primative *primative, float x, float y, float z)
{
  mat4x4_translate(primative->transformation, x, y, z);
}

void primative_scale(Primative *primative, float x, float y, float z)
{
  mat4x4_scale_aniso(primative->transformation, primative->transformation, x, y, z);
}

float primative_distance(Primative *primative, vec4 point)
{
  vec4 local;
  mat4x4 transformation;
  mat4x4_invert(transformation, primative->transformation);
  mat4x4_mul_vec4(local, transformation, point);
  local[3] = 0;

  switch (primative->shape)
  {
    case SPHERE:
    {
      return vec4_len(local) - 0.5;
    }
    case CUBE:
    {
      vec3 p = {
        fabsf(p[0]) - 0.5,
        fabsf(p[1]) - 0.5,
        fabsf(p[2]) - 0.5
      };
      float d = min(max(p[0], max(p[1], p[2])), 0.0);
      p[0] = max(p[0], 0);
      p[1] = max(p[1], 0);
      p[2] = max(p[2], 0);
      return d + vec3_len(p);
    }
    case CYLINDER:
    {
      return 0;
    }
  }
}
