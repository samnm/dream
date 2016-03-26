
#include <assert.h>
#include <stdlib.h>

#include "point.h"

Point* point_create(vec3 pos)
{
  Point *point = malloc(sizeof(Point));
  assert(point != NULL);

  point->pos[0] = pos[0];
  point->pos[1] = pos[1];
  point->pos[2] = pos[2];

  point->color[0] = 1.0;
  point->color[1] = 1.0;
  point->color[2] = 1.0;

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
  vec4 pos = {point->pos[0], point->pos[1], point->pos[2], 1.0};
  vec4 epsX = {eps, 0.f, 0.f, 0.0f};
  vec4 epsY = {0.f, eps, 0.f, 0.0f};
  vec4 epsZ = {0.f, 0.f, eps, 0.0f};

  vec4 ppx, psx, ppy, psy, ppz, psz;
  vec4_add(ppx, pos, epsX);
  vec4_add(ppy, pos, epsY);
  vec4_add(ppz, pos, epsZ);
  vec4_sub(psx, pos, epsX);
  vec4_sub(psy, pos, epsY);
  vec4_sub(psz, pos, epsZ);

  point->norm[0] = sdf(ppx) - sdf(psx);
  point->norm[1] = sdf(ppy) - sdf(psy);
  point->norm[2] = sdf(ppz) - sdf(psz);
  vec3_norm(point->norm, point->norm);
}
