#ifndef SDF_H_
#define SDF_H_

#include <stdbool.h>

#include "linmath.h"
#include "point.h"

typedef enum {
  ADDITIVE,
  SUBTRACTIVE,
} csg_operation;

typedef enum {
  SPHERE,
  CUBE,
  CYLINDER,
} csg_shape;

typedef struct primative_s {
  mat4x4 transformation;
  float blend;
  csg_operation operation;
  csg_shape shape;
} Primative;

Primative *primative_create();
void primative_destroy(Primative *primative);

void primative_translate(Primative *primative, float x, float y, float z);
void primative_scale(Primative *primative, float x, float y, float z);

float primative_distance(Primative *primative, vec4 point);

#endif /* SDF_H_ */
