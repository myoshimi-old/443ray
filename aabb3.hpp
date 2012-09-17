#ifndef _INC_AABB3
#define _INC_AABB3

#include "vector3.hpp"
#include "polygon3.hpp"

class AABB3{
public:
  Vector3 max, min;
  AABB3* right;
  AABB3* left;
  Polygon3* node;
  AABB3(){};
  void empty();
  void add(Vector3 p);
  void show();
  AABB3* clone();
  Vector3 get_gravity_center();
  bool intersect(Vector3 viewpoint, Vector3 view_vector);
  static bool cmpx(AABB3* a, AABB3* b);
};

#endif // _INC_AABB3

