#ifndef _INC_SCENE
#define _INC_SCENE

#include<vector>
#include<list>

#include "vector3.hpp"
#include "polygon3.hpp"
#include "aabb3.hpp"
#include "light.hpp"

using namespace std;

class Scene{
public:
  Color bgcolor;
  Vector3 viewpoint;
  vector<Light*> light;
  vector<Polygon3*> model;
  vector<AABB3*> bvolume;
  AABB3* bbtree;
  int vertex;
  int face;

  void build_bounding_box();
  AABB3* division(int s, int e, int f);
  int partition(int s, int e, int f);
  void qsort(int s, int e, int f);
  void bvolume_sort(int s, int e, int f);
  static unsigned long long int num;
  // コンストラクタ
  Scene(){};
  void set_bgcolor(UINT8 r, UINT8 g, UINT8 b){
    bgcolor.red   = r;
    bgcolor.green = g;
    bgcolor.blue  = b;
  }
  void add_light(Light* m){light.push_back(m);};
  list<Polygon3*> tree_traversal(Vector3 view_vector);
  void load_ply(string filename);
};

#endif // _INC_SCENE
