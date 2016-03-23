
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "octtree.h"
#include "vec3.h"

OctTree* octTree_create(Vec3 origin, Vec3 halfDim)
{
  OctTree *octTree = malloc(sizeof(OctTree));
  assert(octTree != NULL);

  OctTree o = *octTree;
  o = (const struct OctTree){0};
  o.origin = origin;
  o.halfDim = halfDim;

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
        Vec3 newOrigin = {
          octTree->origin.x + octTree->halfDim.x * ((i&4) > 0 ? 0.5 : -0.5),
          octTree->origin.y + octTree->halfDim.y * ((i&2) > 0 ? 0.5 : -0.5),
          octTree->origin.z + octTree->halfDim.z * ((i&1) > 0 ? 0.5 : -0.5),
        };
        Vec3 newHalfDim = {
          octTree->halfDim.x * 0.5,
          octTree->halfDim.y * 0.5,
          octTree->halfDim.z * 0.5
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
  if (point->pos.x >= octTree->origin.x) oct |= 4;
  if (point->pos.y >= octTree->origin.y) oct |= 2;
  if (point->pos.z >= octTree->origin.z) oct |= 1;
  return oct;
}
