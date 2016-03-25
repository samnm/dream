
#include <assert.h>
#include <stdlib.h>

#include "point.h"

Point* point_create()
{
  Point *point = malloc(sizeof(Point));
  assert(point != NULL);
  return point;
}

void point_destroy(Point *point)
{
  assert(point != NULL);
  free(point);
}

void point_calc_normal(Point *point, distance_func sdf)
{
  float eps = 0.1e-4;
  vec3 epsX = {eps, 0.f, 0.f};
  vec3 epsY = {0.f, eps, 0.f};
  vec3 epsZ = {0.f, 0.f, eps};

  vec3 ppx, psx, ppy, psy, ppz, psz;
  vec3_add(ppx, point->pos, epsX);
  vec3_add(ppy, point->pos, epsY);
  vec3_add(ppz, point->pos, epsZ);
  vec3_sub(psx, point->pos, epsX);
  vec3_sub(psy, point->pos, epsY);
  vec3_sub(psz, point->pos, epsZ);

  point->norm[0] = sdf(ppx) - sdf(psx);
  point->norm[1] = sdf(ppy) - sdf(psy);
  point->norm[2] = sdf(ppz) - sdf(psz);
  vec3_norm(point->norm, point->norm);
}
