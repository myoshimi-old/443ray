#ifndef _INC_POLYGON3
#define _INC_POLYGON3

#include "vector3.hpp"
#include "color.hpp"

//class Polygon3 : public Model{
class Polygon3{
public:
  Vector3 v1, v2, v3;
  Color color;
  Polygon3(){};
  Polygon3(Color* set_color, Vector3* set_v1, Vector3* set_v2, Vector3* set_v3);
  REAL intersect(Vector3 viewpoint, Vector3 view_vector);
  Vector3 get_normal_vector(Vector3 np);
  void show();
  Color get_color();
  Vector3 get_max();
  Vector3 get_min();
};

#endif // _INC_POLYGON3
