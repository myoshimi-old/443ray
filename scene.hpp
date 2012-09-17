#ifndef _INC_SCENE
#define _INC_SCENE

#include<vector>
#include<list>
#include<omp.h>

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

  omp_lock_t lock_num;
  omp_lock_t lock_pnum;
  unsigned long long int num;
  unsigned long long int pnum;
  // コンストラクタ
  Scene();
  ~Scene();

  void build_bounding_box();
  AABB3* division(int s, int e, int f);
  int partition(int s, int e, int f);
  void qsort(int s, int e, int f);
  void set_bgcolor(UINT8 r, UINT8 g, UINT8 b);
  void add_light(Light* m);
  Color intersect(Vector3 v);
  Color shading(Vector3 view_vector,
                Vector3 light,
                Vector3 n,
                Color color,
                double s);
  list<Polygon3*> tree_traversal(Vector3 view_vector);
  void load_ply(string filename);
  static list<string> split(string str, string delim);
};

#endif // _INC_SCENE
