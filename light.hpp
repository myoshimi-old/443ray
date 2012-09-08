#ifndef _INC_LIGHT
#define _INC_LIGHT

#include "vector3.hpp"

class Light{
public:
  Vector3 vector;
  Light(){};
  Light(Vector3* sv){
    vector.x = sv->x;
    vector.y = sv->y;
    vector.z = sv->z;
  };
  void set_vector(Vector3 sv){
    vector.x = sv.x;
    vector.y = sv.y;
    vector.z = sv.z;
  };
  Vector3 get_vector(){
    Vector3 v;
    v.x = vector.x;
    v.y = vector.y;
    v.z = vector.z;
    return v;
  }
};

#endif // _INC_LIGHT
