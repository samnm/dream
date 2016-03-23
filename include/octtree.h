
#ifndef OCTTREE_H_
#define OCTTREE_H_

#include <stdbool.h>

#include "linmath.h"
#include "point.h"

typedef struct OctTree {
  vec3 origin;
  vec3 halfDim;
  int depth;

  struct OctTree *parent;
  struct OctTree *children[8];

  Point *data;
} OctTree;

OctTree* octTree_create(vec3 origin, vec3 halfDim);
void octTree_destroy(OctTree *octTree);

bool octTree_isLeaf(OctTree *octTree);

void octTree_insert(OctTree *octTree, Point *point);

int octTree_octantContainingPoint(OctTree *octTree, Point *point);

#endif /* OCTTREE_H_ */
