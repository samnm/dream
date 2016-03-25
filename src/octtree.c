
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "linmath.h"
#include "octtree.h"
#include "point.h"

OctTree* octTree_create(vec3 origin, vec3 halfDim)
{
  OctTree *octTree = malloc(sizeof(OctTree));
  assert(octTree != NULL);

  octTree->depth = 0;
  octTree->parent = NULL;
  for (int i = 0; i < 8; ++i)
  {
    octTree->children[i] = NULL;
  }
  octTree->data = NULL;

  octTree->origin[0] = origin[0];
  octTree->origin[1] = origin[1];
  octTree->origin[2] = origin[2];

  octTree->halfDim[0] = halfDim[0];
  octTree->halfDim[1] = halfDim[1];
  octTree->halfDim[2] = halfDim[2];

  return octTree;
}

void octTree_destroy(OctTree *octTree)
{
  assert(octTree != NULL);

  if (octTree->children[0] != NULL)
  {
    for (int i = 0; i < 8; ++i)
      octTree_destroy(octTree->children[i]);

    free(octTree->children);
  }

  free(octTree);
}

bool octTree_isLeaf(OctTree *octTree)
{
  return octTree->children[0] == NULL;
}

int octTree_count(OctTree *octTree)
{
  int count = (octTree->data != NULL) ? 1 : 0;
  if (!octTree_isLeaf(octTree))
  {
    for (int i = 0; i < 8; ++i)
      count += octTree_count(octTree->children[i]);
  }
  return count;
}

void octTree_get_points(OctTree *octTree, Point *points_p, int *i_p)
{
  if (octTree->data != NULL)
  {
    points_p[*i_p] = *octTree->data;
    i_p[0] = i_p[0] + 1;
  }
  if (!octTree_isLeaf(octTree))
  {
    for (int i = 0; i < 8; ++i)
      octTree_get_points(octTree->children[i], points_p, i_p);
  }
}

void octTree_insert(OctTree *octTree, Point *point)
{
  if (octTree_isLeaf(octTree))
  {
    if (octTree->data == NULL)
    {
      octTree->data = point;
    }
    else
    {
      Point *oldData = octTree->data;

      for (int i = 0; i < 8; ++i)
      {
        vec3 newOrigin = {
          octTree->origin[0] + octTree->halfDim[0] * ((i&4) > 0 ? 0.5 : -0.5),
          octTree->origin[1] + octTree->halfDim[1] * ((i&2) > 0 ? 0.5 : -0.5),
          octTree->origin[2] + octTree->halfDim[2] * ((i&1) > 0 ? 0.5 : -0.5),
        };
        vec3 newHalfDim = {
          octTree->halfDim[0] * 0.5,
          octTree->halfDim[1] * 0.5,
          octTree->halfDim[2] * 0.5
        };

        OctTree *child = octTree_create(newOrigin, newHalfDim);
        child->parent = octTree;
        child->depth = octTree->depth + 1;
        octTree->children[i] = child;
      }

      OctTree *tree = octTree->children[octTree_octantContainingPoint(octTree, point)];
      octTree_insert(tree, point);

      tree = octTree->children[octTree_octantContainingPoint(octTree, oldData)];
      octTree_insert(tree, oldData);
    }
  }
  else
  {
    OctTree *tree = octTree->children[octTree_octantContainingPoint(octTree, point)];
    octTree_insert(tree, point);
  }
}

void octTree_populate(OctTree *octTree, distance_func sdf)
{
  float thresh = 0.001;
  int maxDepth = 8;
  float distance = sdf(octTree->origin);
  float abs_dist = fabsf(distance);

  if (octTree->depth < maxDepth && abs_dist < vec3_len(octTree->halfDim))
  {
    vec3 new_half_dim;
    vec3_scale(new_half_dim, octTree->halfDim, 0.5);

    for (int i = 0; i < 8; ++i)
    {
      vec3 new_origin = {
        octTree->origin[0] + octTree->halfDim[0] * ((i&4) > 0 ? 0.5 : -0.5),
        octTree->origin[1] + octTree->halfDim[1] * ((i&2) > 0 ? 0.5 : -0.5),
        octTree->origin[2] + octTree->halfDim[2] * ((i&1) > 0 ? 0.5 : -0.5),
      };
      OctTree *child = octTree_create(new_origin, new_half_dim);
      child->depth = octTree->depth + 1;
      child->parent = octTree;
      octTree_populate(child, sdf);
      octTree->children[i] = child;
    }
  }
  else if (octTree->depth == maxDepth || abs_dist <= thresh)
  {
    Point *point = point_create(octTree->origin);
    point_calc_normal(point, sdf);
    octTree->data = point;
  }
}

int octTree_octantContainingPoint(OctTree *octTree, Point *point)
{
  int oct = 0;
  if (point->pos[0] >= octTree->origin[0]) oct |= 4;
  if (point->pos[1] >= octTree->origin[1]) oct |= 2;
  if (point->pos[2] >= octTree->origin[2]) oct |= 1;
  return oct;
}
