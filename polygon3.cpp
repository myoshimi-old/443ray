#include<iostream>
#include<cmath>

#include "443ray.hpp"
#include "polygon3.hpp"
#include "color.hpp"
#include "vector3.hpp"

using namespace std;

// 物体定義の抽象クラス
/*
class Model{
public:
  // = 0 をつけてメソッドの定義を省略する
  virtual REAL intersect(Vector3 viewpoint, Vector3 view_vector) = 0;
  virtual Vector3 get_normal_vector(Vector3 np) = 0;
  virtual void show() = 0;
  virtual Color get_color()=0;
  virtual Vector3 get_max()=0;
  virtual Vector3 get_min()=0;
};
*/

Polygon3::Polygon3(Color* set_color,
                   Vector3* set_v1,
                   Vector3* set_v2,
                   Vector3* set_v3){
  color = *set_color;
  v1 = *set_v1;
  v2 = *set_v2;
  v3 = *set_v3;
};

bool Polygon3::cmp(Polygon3* a, Polygon3* b){
  bool ret=false;
  if(a->v1.x < b->v1.x){
    ret = true;
  }
  return ret;
}

Vector3 Polygon3::get_max(){
  Vector3 ret;
  REAL x, y, z;
  x = v1.x;
  if(x < v2.x) x = v2.x;
  if(x < v3.x) x = v3.x;
  y = v1.y;
  if(y < v2.y) y = v2.y;
  if(y < v3.y) y = v3.y;
  z = v1.z;
  if(z < v2.z) z = v2.z;
  if(z < v3.z) z = v3.z;
  ret.set_vector(x, y, z);
  return ret;
};

Vector3 Polygon3::get_min(){
  Vector3 ret;
  REAL x, y, z;
  x = v1.x;
  if(x > v2.x) x = v2.x;
  if(x > v3.x) x = v3.x;
  y = v1.y;
  if(y > v2.y) y = v2.y;
  if(y > v3.y) y = v3.y;
  z = v1.z;
  if(z > v2.z) z = v2.z;
  if(z > v3.z) z = v3.z;
  ret.set_vector(x, y, z);
  return ret;
};

REAL Polygon3::intersect(Vector3 viewpoint, Vector3 view_vector){
  Vector3 t0, t1, n, vp;
  Vector3 d0, d1, c1;
  REAL nv, xp, t, ret;
  t0 = v2 - v1;
  t1 = v3 - v1;
  n = Vector3::cross(t1, t0);
  n.normalize();

  nv = Vector3::dot(n, view_vector);
  // 視線と三角形平面との交差判定
  ret = HUGE_VAL;
  // 視線が三角形と平行な場合
  if(nv > 0.00001){
    ret = HUGE_VAL;
  }
  else{
    xp = Vector3::dot((v1 - viewpoint), n);
    t = xp / nv;

    // 後ろ向きのレイは無効
    ret = HUGE_VAL;
    if(t < 0.0){
      vp = viewpoint + view_vector * t;

      d0 = vp - v1;
      d1 = v2 - v1;
      c1 = Vector3::cross(d0, d1);
      if(Vector3::dot(c1, n) > 0.0){
        d0 = vp - v2;
        d1 = v3 - v2;
        c1 = Vector3::cross(d0, d1);
        if(Vector3::dot(c1, n) > 0.0){
          d0 = vp - v3;
          d1 = v1 - v3;
          c1 = Vector3::cross(d0, d1);
          if(Vector3::dot(c1, n) > 0.0){
            ret = t;
          }
        }
      }
    }
  }
  return ret;
};

Vector3 Polygon3::get_normal_vector(Vector3 np){
  Vector3 t0, t1, n;

  t0 = v2 - v1;
  t1 = v3 - v1;
  n  = Vector3::cross(t0, t1);
  n.normalize();

  return n;
};

void Polygon3::show(){
  cout << "Polygon3 : " << endl;
  cout << "V1 ["<< v1.x << ", " << v1.y << ", " << v1.z << "]" << endl;
  cout << "V2 ["<< v2.x << ", " << v2.y << ", " << v2.z << "]" << endl;
  cout << "V3 ["<< v3.x << ", " << v3.y << ", " << v3.z << "]" << endl;
  cout << "C  ("<< (int)color.red
       << ", " << (int)color.green
       << ", " << (int)color.blue
       << ")" << endl;
};

Color Polygon3::get_color(){
  Color c;
  c.set_color(color.red, color.green, color.blue);
  return c;
};

