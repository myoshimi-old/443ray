#include"vector3.hpp"

#include<iostream>
#include<cmath>

using namespace std;

Vector3::Vector3(REAL set_x, REAL set_y, REAL set_z){
  x = set_x;
  y = set_y;
  z = set_z;
};

void Vector3::set_vector(REAL set_x, REAL set_y, REAL set_z){
  x = set_x;
  y = set_y;
  z = set_z;
};

void Vector3::normalize(){
  REAL d;
  d = sqrt(x * x + y * y + z * z);
  x /= d;
  y /= d;
  z /= d;
};

void Vector3::show(){
  cout << "[" << x << ", " << y << ", " << z << "]" << endl;
};

Vector3 Vector3::operator+(Vector3 v){
  Vector3 ret_vector;
  ret_vector.set_vector(x+v.x, y+v.y, z+v.z);
  return ret_vector;
};

Vector3 Vector3::operator-(Vector3 v){
  Vector3 ret_vector;
  ret_vector.set_vector(x-v.x, y-v.y, z-v.z);
  return ret_vector;
};

Vector3 Vector3::operator*(REAL t){
  Vector3 ret_vector;
  ret_vector.set_vector(x*t, y*t, z*t);
  return ret_vector;
};

Vector3 Vector3::cross(Vector3 a, Vector3 b){
  Vector3 ret_vector;
  ret_vector.set_vector(a.y * b.z - b.y * a.z,
                        a.z * b.x - b.z * a.x,
                        a.x * b.y - b.x * a.y);
  return ret_vector;
};

REAL Vector3::dot(Vector3 a, Vector3 b){
  return a.x * b.x + a.y * b.y + a.z * b.z;
};
