#ifndef _INC_SCREEN
#define _INC_SCREEN

#include "443ray.hpp"
#include "vector3.hpp"
#include "color.hpp"
#include<string>

using namespace std;

class Screen{
public:
  UINT32 width;
  UINT32 height;
  Vector3 vector;
  Color* pixel;
  // コンストラクタ
  Screen(){};
  // メソッド
  void set_screen(UINT32 set_width, UINT32 set_height);
  // 画像の書き出しメソッド
  void write_ppm(string file);
  void write_png(string file);
};

#endif // _INC_SCREEN
