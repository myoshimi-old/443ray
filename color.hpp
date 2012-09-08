#ifndef _INC_COLOR
#define _INC_COLOR

class Color{
public:
  UINT8 red;
  UINT8 green;
  UINT8 blue;
  // コンストラクタ
  Color(){};
  Color(UINT8 set_red, UINT8 set_green, UINT8 set_blue);
  // メソッド
  void set_color(Color c);
  void set_color(UINT8 red, UINT8 green, UINT8 blue);
  // *演算子の再定義: RGB値に実数tを乗算
  Color operator*(REAL t);
  // +演算子の再定義: RGB値同士の加算
  Color operator+(Color c);
};

#endif // _INC_COLOR
