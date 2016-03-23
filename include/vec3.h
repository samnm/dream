
#ifndef VEC3_H_
#define VEC3_H_

typedef struct {
  union {
    struct {
      float x;
      float y;
      float z;
    };
    float xyz[3];
  };
} Vec3;

#endif /* VEC3_H_ */
