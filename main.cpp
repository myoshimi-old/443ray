#include<iostream>
#include<ctime>
#include<cmath>
#include<fstream>
#include<vector>
#include<list>

#include<sys/time.h>

#include "443ray.hpp"
#include "vector3.hpp"
#include "polygon3.hpp"
#include "aabb3.hpp"
#include "color.hpp"
#include "screen.hpp"
#include "light.hpp"
#include "scene.hpp"

using namespace std;

unsigned long long int pnum = 0;

double get_dtime(void){
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return ((double)(tv.tv_sec) + (double)(tv.tv_usec) * 0.001 * 0.001);
}

Color shading(Vector3 view_vector,
              Vector3 light,
              Vector3 n,
              Color color,
              double s){
  REAL kd = 0.7, ks = 0.7, ke = 0.3;
  Color c, c0, c1;
  REAL ln, lv, nv;
  REAL cos_alpha, cos_beta, cos_beta_pow;
  REAL h;
  // c.set_color(200, 200, 200);

  ln = Vector3::dot(light, n);
  lv = Vector3::dot(light, view_vector);
  nv = Vector3::dot(n, view_vector);

  cos_alpha = ln * (-1.0);
  if(cos_alpha < 0.0){
    cos_alpha = 0.0;
  }

  cos_beta = ln * 2.0 * nv - lv;
  if(cos_beta < 0.0){
    cos_beta = 0.0;
  }

  cos_beta_pow = pow(cos_beta, 20.0);

  h = (s * kd * cos_alpha + ke);
  c0 = color * h;
  c1.set_color(255,255,255); // 環境光
  c1 = c1 * (s * ks * cos_beta_pow);
  c = c0 + c1;
  return c;
};

// view : スクリーンの画素位置へのベクトル
Color intersect(Vector3 view, Scene scene){
  int m;
  REAL dv, t;
  Vector3 view_vector; // 視線(単位)ベクトル
  vector<pair<Polygon3*, double> > xlist; // 交差したモデルのリスト
  Color c;
  Polygon3* min_id;
  REAL min;
  Vector3 tv, np, n;
  Vector3 light;
  list<Polygon3*> mlist;
  Polygon3* tm;

  // 視線(単位)ベクトルの計算
  dv = sqrt(view.x * view.x + view.y * view.y + view.z * view.z);
  view_vector.set_vector(scene.viewpoint.x + view.x,
                         scene.viewpoint.y + view.y,
                         scene.viewpoint.z + (1.0 * dv));
  view_vector = view_vector - scene.viewpoint;
  view_vector.normalize();

  // バウンディングボックスの探索
  // ボックスの交差判定
  // tree[0].node.x
  xlist.empty();

  mlist = scene.tree_traversal(view_vector);

  // cout << "Polygon3 intersect : " << (int)mlist.size() << endl;

  // 交差判定
  for(m = 0; m<(int)mlist.size(); m++){
    tm = mlist.front();
    pnum++;
    t = tm->intersect(scene.viewpoint, view_vector);
    if(t < HUGE_VAL){
      xlist.push_back(pair<Polygon3*, double>(tm, t));
    }
    mlist.pop_front();
    mlist.push_back(tm);
  }
  /*
  for(m = 0; m<(int)scene.model.size(); m++){
    t = scene.model[m]->intersect(scene.viewpoint, view_vector);
    if(t < HUGE_VAL){
      xlist.push_back(pair<int, double>(m, t));
    }
  }
  */

  if(xlist.size() == 0){
    c.set_color(scene.bgcolor.red,
                scene.bgcolor.green,
                scene.bgcolor.blue);
  }
  else{
    // 直近のモデルを探索
    min_id = xlist[0].first;
    min    = xlist[0].second;
    for(m = 0; m<(int)xlist.size();m++){
      if(min > xlist[m].second){
        min_id = xlist[m].first;
        min = xlist[m].second;
      }
    }
    // 視線ベクトルをスカラー倍したベクトル
    tv = view_vector * min;
    // 交点(視点から交点へのベクトル)
    np = scene.viewpoint + tv;
    // 物体の法線ベクトル(単位ベクトル)の計算
    // n  = scene.model[min_id]->get_normal_vector(np);
    n  = min_id->get_normal_vector(np);
    // 光線ベクトル
    light = np - scene.light[0]->get_vector();
    light.normalize();
    c = shading(view_vector,
                light, n, min_id->get_color(),
                0.5);
  }
  return c;
};

unsigned long long int Scene::num = 0;

int main(int argc, char** argv){
  int x, y;
  Screen screen;
  Scene scene;

  ifstream ifs;
  string s;
  list<string> slist;
  Vector3 v;
  vector<Vector3> vertex_list;
  Vector3 sl; // スクリーン位置
  REAL rh, rw;
  double d0, d1, d2, d3;

  if(argc != 5){
    cout << argv[0] << " [width] [height] [.ply file] [.ppm file]" << endl;
    exit(1);
  }

  screen.set_screen(atoi(argv[1]), atoi(argv[2]));
  scene.set_bgcolor(0, 0, 0);
  // scene.viewpoint.set_vector(0.3, -0.2, -2.85); // bunny背面
  // scene.viewpoint.set_vector(-0.02, 0.145, 0.40); // 視点位置
  //scene.add_light(new Light(new Vector3(600.0, 600.0, -600.0)));
  scene.add_light(new Light(new Vector3(60.0, 30.0, 60.0)));

  scene.load_ply(argv[3]);

  // 縦横比
  rw = 0.16;
  rh = rw * screen.height / screen.width;
  sl.set_vector(rw, rh, 0.6);
  // ウサギ
  //scene.viewpoint.set_vector(-0.015, 0.104, 0.80); // 視点位置
  // 仏像
  scene.viewpoint.set_vector(-0.015, 0.140, 1.10); // 視点位置

  // view_{xyz}はスクリーンのワールド座標
  vector<Vector3> view;
  view.resize(screen.height*screen.width);
  for(y=0;y<(int)screen.height;y++){
    for(x=0;x<(int)screen.width;x++){
      view[sindex].set_vector(((x * sl.x / screen.width) - sl.x/2.0),
                              (y * sl.y / screen.height - sl.y/2.0),
                              sl.z);
    }
  }

  Vector3 viewpoint;   // 視点ベクトル
  viewpoint.set_vector(scene.viewpoint.x,
                       scene.viewpoint.y,
                       scene.viewpoint.z);

  cout << "ply, " << argv[3] << endl;
  cout << "width  : ,     " << screen.width  << endl;
  cout << "height : ,     " << screen.height << endl;
  cout << "vertex : ,     " << scene.vertex << endl;
  cout << "face   : ,     " << scene.face   << endl;

  cout << "Building BoundingBox..." << endl;
  d0 = get_dtime();
  scene.build_bounding_box();

  cout << "Intersect..." << endl;
  d1 = get_dtime();

#ifdef _OPENMP
#pragma omp parallel for private(x)
#endif
  for(y=0;y<(int)screen.height;y++){
    for(x=0;x<(int)screen.width;x++){
      // cout << "(" << x << ", " << y << ")" << endl;
      screen.pixel[sindex].set_color(intersect(view[sindex], scene));
    }
  }

  d2 = get_dtime();
  screen.write_png(argv[4]);
  //screen.write_ppm(argv[4]);

  d3 = get_dtime();

  cout << "traversal, " << Scene::num << endl;
  cout << "intersect, " << pnum << endl;

  cout << "Building VBH : ,     " << d1 - d0 << ", [sec]" << endl;
  cout << "Intersect    : ,     " << d2 - d1 << ", [sec]" << endl;
  cout << "Generation   : ,     " << d3 - d2 << ", [sec]" << endl;
  cout << "Total        : ,     " << d3 - d0 << ", [sec]" << endl;

  return 0;
}
