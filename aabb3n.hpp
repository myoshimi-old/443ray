#ifndef _INC_AABB3N
#define _INC_AABB3N

#include<vector>

#include "vector3.hpp"
#include "polygon3.hpp"

using namespace std;

class AABB3N{
public:
    Vector3 max;
    Vector3 min;
    vector<Polygon3*> model;
    AABB3N(){};
    void empty();
    void add(Vector3 p);
    void show();
    AABB3N* clone();
    Vector3 get_gravity_center();
    bool intersect(Vector3 viewpoint, Vector3 view_vector);
    static bool cmpx(AABB3N* a, AABB3N* b);
};

#endif // _INC_AABB3N

