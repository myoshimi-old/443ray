#include "light.hpp"

Light::Light(Vector3* sv){
  vector.x = sv->x;
  vector.y = sv->y;
  vector.z = sv->z;
};

void Light::set_vector(Vector3 sv){
  vector.x = sv.x;
  vector.y = sv.y;
  vector.z = sv.z;
};

Vector3 Light::get_vector(){
  Vector3 v;
  v.x = vector.x;
  v.y = vector.y;
  v.z = vector.z;
  return v;
};
