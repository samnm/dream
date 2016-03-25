
#ifndef POINT_H_
#define POINT_H_

#include "linmath.h"

typedef float (*distance_func)(vec4 point);

typedef struct point_s {
  vec3 pos;
  vec3 norm;
  vec3 color;
} Point;

Point* point_create(vec3 pos);
void point_destroy(Point *point);

void point_calc_normal(Point *point, distance_func sdf);

#endif /* POINT_H_ */
