#ifndef _INC_VECTOR3
#define _INC_VECTOR3

#include "443ray.hpp"

class Vector3{
public:
  REAL x, y, z;
  // コンストラクタ
  Vector3(){};
  Vector3(REAL set_x, REAL set_y, REAL set_z);
  // メソッド
  void set_vector(REAL set_x, REAL set_y, REAL set_z);
  void normalize();
  void show();
  // +演算子の再定義: ベクトル同士の加算
  Vector3 operator+(Vector3 v);
  // -演算子の再定義: ベクトル同士の減算
  Vector3 operator-(Vector3 v);
  // *演算子の再定義: ベクトルに実数tを乗算
  Vector3 operator*(REAL t);
  // クラスメソッド
  static Vector3 cross(Vector3 a, Vector3 b); // 外積
  static REAL dot(Vector3 a, Vector3 b); // 内積
};

#endif // _INC_VECTOR3
