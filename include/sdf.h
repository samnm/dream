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

float primative_distance(Primative *primative, vec4 point);

#endif /* SDF_H_ */
