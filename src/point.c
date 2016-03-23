
#include <assert.h>
#include <stdlib.h>

#include "point.h"

Point* point_create() {
  Point *point = malloc(sizeof(Point));
  assert(point != NULL);
  return point;
}

void point_destroy(Point *point) {
  assert(point != NULL);
  free(point);
}
