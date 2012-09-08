#ifndef _INC_LIGHT
#define _INC_LIGHT

#include "vector3.hpp"

class Light{
public:
  Vector3 vector;
  Light(){};
  Light(Vector3* sv);
  void set_vector(Vector3 sv);
  Vector3 get_vector();
};

#endif // _INC_LIGHT
