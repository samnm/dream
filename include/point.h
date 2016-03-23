
#ifndef POINT_H_
#define POINT_H_

#include "linmath.h"

typedef struct point_s {
  vec3 pos;
  vec3 norm;
  float dist;
  vec3 color;
} Point;

Point* point_create();
void point_destroy(Point *point);

#endif /* POINT_H_ */
