
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "linmath.h"
#include "octtree.h"

OctTree* octTree_create(vec3 origin, vec3 halfDim)
{
  OctTree *octTree = malloc(sizeof(OctTree));
  assert(octTree != NULL);

  OctTree o = *octTree;
  o = (const struct OctTree){0};
  o.origin[0] = origin[0];
  o.origin[1] = origin[1];
  o.origin[2] = origin[2];
  o.halfDim[0] = halfDim[0];
  o.halfDim[1] = halfDim[1];
  o.halfDim[2] = halfDim[2];

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

int octTree_octantContainingPoint(OctTree *octTree, Point *point)
{
  int oct = 0;
  if (point->pos[0] >= octTree->origin[0]) oct |= 4;
  if (point->pos[1] >= octTree->origin[1]) oct |= 2;
  if (point->pos[2] >= octTree->origin[2]) oct |= 1;
  return oct;
}
