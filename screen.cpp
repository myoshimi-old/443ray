#include<iostream>
#include<string>
#include<cstdlib>
#include<fstream>
#include<cv.h>
#include<highgui.h>

#include "443ray.hpp"
#include "screen.hpp"

using namespace std;
using namespace cv;

void Screen::set_screen(UINT32 set_width, UINT32 set_height){
  int i, j;

  width  = set_width;
  height = set_height;

  pixel = new Color[height * width];

  for(j=0; j<(int)height; j++){
    for(i=0; i<(int)width; i++){
      pixel[j * width + i].set_color(0,0,0);
    }
  }
};

void Screen::write_ppm(string file){
  ofstream ofs;
  int i, j;

  ofs.open(file.c_str());

  if(ofs){
    ofs << "P3" << endl;
    ofs << "# " << file.c_str() << endl;
    ofs << width << " " << height << endl;
    ofs << "255" << endl;
    for(j=0;j<(int)height;j++){
      for(i=0;i<(int)width;i++){
        ofs << (int)pixel[j * width + i].red << " "
            << (int)pixel[j * width + i].green << " "
            << (int)pixel[j * width + i].blue  << endl;
      }
    }
  }
  ofs.close();
};

void Screen::write_png(string file){
  int x, y;
  Mat mat;
  Mat3b m;
  IplImage output;

  mat.create(height, width, CV_8UC3);
  m = mat;

  for(y=0;y<(int)height;y++){
    for(x=0;x<(int)width;x++){
      m(Point(x, y)) = Vec3b((int)pixel[y * width + x].blue,
                             (int)pixel[y * width + x].green,
                             (int)pixel[y * width + x].red);
    }
  }

  output = m;
  cvSaveImage(file.c_str(), &output);
}

