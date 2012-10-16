#include<iostream>
#include<cmath>
#include<ctime>
#include<fstream>
#include<vector>
#include<list>
#include<omp.h>

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

double get_dtime(void){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((double)(tv.tv_sec)
            + (double)(tv.tv_usec) * 0.001 * 0.001);
}

int main(int argc, char** argv){
    int x, y;
    Screen screen;
    Scene scene;
    Scene* scene1;

    ifstream ifs;
    string s;
    list<string> slist;
    Vector3 v;
    vector<Vector3> vertex_list;
    Vector3 sl; // スクリーン位置
    REAL rh, rw;
    double d0, d1, d2, d3, d4;

    if(argc != 5){
        cout << argv[0] << " [width] [height] [file.ply] [file.{ppm,png}]" << endl;
        exit(1);
    }

    screen.set_screen(atoi(argv[1]), atoi(argv[2]));
    scene.set_bgcolor(0, 0, 0);
    scene.add_light(new Light(new Vector3(60.0, 30.0, 60.0))); // 照明位置


    cout << "Loading PLY File..." << endl;
    d0 = get_dtime();
    scene.load_ply(argv[3]);
    //exit(0);
    // 縦横比
    rw = 0.16;
    rh = rw * screen.height / screen.width;
    sl.set_vector(rw, rh, 0.6); // スクリーン位置
    // ウサギ
    //scene.viewpoint.set_vector(-0.015, 0.104, 0.80); // 視点位置
    // scene.viewpoint.set_vector(-0.02, 0.145, 0.40); // 視点位置
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
    viewpoint.set_vector(scene.viewpoint);

    cout << "ply, " << argv[3] << endl;
    cout << "width  : ,     " << screen.width  << endl;
    cout << "height : ,     " << screen.height << endl;
    cout << "vertex : ,     " << scene.vertex << endl;
    cout << "face   : ,     " << scene.face   << endl;

    cout << "Building BoundingBox..." << endl;
    d1 = get_dtime();
    scene.build_bounding_box();

    scene1 = &scene;

    cout << "Intersect..." << endl;
    d2 = get_dtime();

#ifdef _OPENMP
#pragma omp parallel for private(x)
#endif
    for(y=0;y<(int)screen.height;y++){
        for(x=0;x<(int)screen.width;x++){
            // cout << "(" << x << ", " << y << ")" << endl;
            screen.pixel[sindex].set_color(scene1->intersect(view[sindex]));
        }
    }

    d3 = get_dtime();
    screen.write_png(argv[4]);
    //screen.write_ppm(argv[4]);

    d4 = get_dtime();

    cout << "traversal, " << scene.num << endl;
    cout << "intersect, " << scene.pnum << endl;

    cout << "Loading PLY  : ,     " << d1 - d0 << ", [sec]" << endl;
    cout << "Building BVH : ,     " << d2 - d1 << ", [sec]" << endl;
    cout << "Intersect    : ,     " << d3 - d2 << ", [sec]" << endl;
    cout << "Generation   : ,     " << d4 - d3 << ", [sec]" << endl;
    cout << "Total        : ,     " << d4 - d0 << ", [sec]" << endl;

    return 0;
}
