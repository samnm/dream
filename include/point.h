
#ifndef POINT_H_
#define POINT_H_

#include "vec3.h"

typedef struct point_s {
  Vec3 pos;
  Vec3 norm;
  float dist;
  Vec3 color;
} Point;

Point* point_create();
void point_destroy(Point *point);

#endif /* POINT_H_ */
