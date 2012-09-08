#include "443ray.hpp"
#include "color.hpp"

Color::Color(UINT8 set_red, UINT8 set_green, UINT8 set_blue){
  red   = set_red;
  green = set_green;
  blue  = set_blue;
};


void Color::set_color(Color c){
  red = c.red;
  green = c.green;
  blue = c.blue;
};

void Color::set_color(UINT8 r, UINT8 g, UINT8 b){
  red   = r;
  green = g;
  blue  = b;
};

Color Color::operator*(REAL t){
  Color ret_color;
  ret_color.set_color((UINT8)(red   * t)%256,
                      (UINT8)(green * t)%256,
                      (UINT8)(blue  * t)%256);
  return ret_color;
};

Color Color::operator+(Color c){
  Color ret_color;
  ret_color.set_color((UINT8)(red   + c.red)%256,
                      (UINT8)(green + c.green)%256,
                      (UINT8)(blue  + c.blue)%256);
  return ret_color;
};
