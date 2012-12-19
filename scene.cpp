#include<iostream>
#include<vector>
#include<list>
#include<fstream>
#include<omp.h>
#include<cmath>

#include "443ray.hpp"
#include "scene.hpp"

Scene::Scene(){
#ifdef _OPENMP
    omp_init_lock(&lock_num);
    omp_init_lock(&lock_pnum);
#endif
    num  = 0;
    pnum = 0;
};

Scene::~Scene(){
#ifdef _OPENMP
    omp_destroy_lock(&lock_num);
    omp_destroy_lock(&lock_pnum);
#endif
};

Scene* Scene::clone(){
    Scene* s;
    s = new Scene();
    s->viewpoint = viewpoint;
    s->bgcolor   = bgcolor;
    s->vertex = vertex;
    s->face   = face;

    s->light.reserve(light.size());
    copy(light.begin(),
         light.end(),
         back_inserter(s->light));
    s->model.reserve(model.size());
    copy(model.begin(),
         model.end(),
         back_inserter(s->model));
    s->bvolume.reserve(bvolume.size());
    copy(bvolume.begin(),
         bvolume.end(),
         back_inserter(s->bvolume));

    s->build_bounding_box();
    
    return s;
};


void Scene::add_light(Light* m){
    light.push_back(m);
};

void Scene::set_bgcolor(UINT8 r, UINT8 g, UINT8 b){
    bgcolor.red   = r;
    bgcolor.green = g;
    bgcolor.blue  = b;
};

/*
  list<string> Scene::split(string str, string delim){
  list<string> result;
  int cutAt;

  while((cutAt = str.find_first_of(delim)) != (int)str.npos){
  if(cutAt>0){
  result.push_back(str.substr(0, cutAt));
  }
  str = str.substr(cutAt+1);
  }
  if(str.length() > 0){
  result.push_back(str);
  }
  return result;
  }
*/

vector<string> Scene::split(string str, string delim){
    vector<string> result;
    int cutAt;
    
    while((cutAt = str.find_first_of(delim)) != (int)str.npos){
        if(cutAt>0){
            result.push_back(str.substr(0, cutAt));
        }
        str = str.substr(cutAt+1);
    }
    if(str.length() > 0){
        result.push_back(str);
    }
    return result;
}

int Scene::split2(vector<string> *svector, char* str, string delim){
    int i;
    char *tp;

    i=0;
    tp = strtok(str, delim.c_str());
    (*svector)[i] = string(tp);
    while(tp != NULL){
        tp = strtok(NULL, delim.c_str());
        if(tp != NULL){
            i++;
            (*svector)[i] = string(tp);
        }
    }

    return i;
}

list<Polygon3*> Scene::tree_traversal2(Vector3 view_vector){
    int tnum;
    int i, idx;
    list<int> traverse_list; // トラバース対象ノードのリスト
    list<Polygon3*> model_list; // 交差判定対象モデルのリスト

    tnum = 0;

    traverse_list.clear();
    model_list.clear();

    tnum++;
    if(btree[0].intersect(viewpoint, view_vector)){
        traverse_list.push_back((int)0);
    }
    
    // cout << "hage " << (int)btree.size() << endl;
    
    while(traverse_list.size() != 0){
        idx = traverse_list.front();
        traverse_list.pop_front();
        /*
        cout << "btree[" << idx << "] : "
             << btree[idx].model.size() << " : "
             << traverse_list.size()
             << endl;
             */

        if(btree[idx].model.size() == 0){
            tnum += child_num;
            for(i=1;i<=child_num;i++){
                if(btree[idx*child_num+i].intersect(viewpoint, view_vector)){
                    // cout << "  Push : " << idx*child_num+i << endl;
                    traverse_list.push_back(idx*child_num+i);
                }
            }
        }
        else{
            for(i=0;i<(int)btree[idx].model.size();i++){
                //tnum++;
                /*
                if(btree[idx].model[i]->intersect(viewpoint, view_vector)){
                    model_list.push_back(btree[idx].model[i]);
                }
                */
                model_list.push_back(btree[idx].model[i]);
            }
        }
    }

    // cout << "hoge" << endl;
#ifdef _OPENMP
    omp_set_lock(&lock_num);
    num += tnum;
    omp_unset_lock(&lock_num);
#else
    num += tnum;
#endif
    
    return model_list;
}

list<Polygon3*> Scene::tree_traversal(Vector3 view_vector){
    int tnum;
    AABB3* tree_ptr;
    list<AABB3*> traverse_list; // トラバース対象ノードのリスト
    list<Polygon3*> model_list; // 交差判定対象モデルのリスト

    tree_ptr = bbtree;

    traverse_list.clear();
    model_list.clear();

    tnum = 0;

    tnum++;
    if(tree_ptr->intersect(viewpoint, view_vector)){
        traverse_list.push_back(tree_ptr);
    }
    while(traverse_list.size() != 0){
        tree_ptr = traverse_list.front();
        traverse_list.pop_front();

        // cout << traverse_list.size() << endl;
        // 葉だったら(モデルがくっついていたら)
        if(tree_ptr->node != NULL){
            // tree_ptr->node->max.show();
            // cout << "add model : " << tree_ptr->node << endl;
            model_list.push_back(tree_ptr->node);
        }
        // 節の(葉でない)場合の処理
        else{
            // 右の子(box)が存在する場合の処理
            if(tree_ptr->right != NULL){
                // 右の子と交差判定
                tnum++;
                if(tree_ptr->right->intersect(viewpoint, view_vector)){
                    // cout << "add right" << endl;
                    traverse_list.push_back(tree_ptr->right);
                }
            }

            if(tree_ptr->left  != NULL){
                // 左の子と交差判定
                tnum++;
                if(tree_ptr->left->intersect(viewpoint, view_vector)){
                    // cout << "add left" << endl;
                    traverse_list.push_back(tree_ptr->left);
                }
            }
        }
    }

    // cout << tnum << endl;
#ifdef _OPENMP
    omp_set_lock(&lock_num);
    num += tnum;
    omp_unset_lock(&lock_num);
#else
    num += tnum;
#endif
    // cout << "box intersect : " << num << endl;

    return model_list;
}

int Scene::partition(int s, int e, int f){
    int right, left;
    REAL pivot;
    AABB3* t;
    Vector3 v1, v2;
    REAL a, b, c;

    if(f % 3 == 0){
        a = bvolume[(e-s)/2+s]->get_gravity_center().x;
        b = bvolume[s]->get_gravity_center().x;
        c = bvolume[e-1]->get_gravity_center().x;
    }
    else if(f % 3 == 1){
        // pivot = bvolume[(e-s)/2+s]->get_gravity_center().y;
        a = bvolume[(e-s)/2+s]->get_gravity_center().y;
        b = bvolume[s]->get_gravity_center().y;
        c = bvolume[e-1]->get_gravity_center().y;
    }
    else{
        a = bvolume[(e-s)/2+s]->get_gravity_center().z;
        b = bvolume[s]->get_gravity_center().z;
        c = bvolume[e-1]->get_gravity_center().z;
    }

    if(a < b){
        if(c < a) pivot = a;
        else // a < c
            if(b < c) pivot = b;
            else      pivot = c;
    }
    else{ // b < a
        if(c < b) pivot = b;
        else // b < c
            if(a < c) pivot = a;
            else      pivot = c;
    }


    //pivot = (a + b + c) / 3.0;
    
    left  = s;
    right = e - 1;
    /*
      cout << "partition" << "(" << s << ", " << e << ")" << endl;
      cout << "   pivot[" << s << "] : " << pivot << endl;
      cout << "     left[" << left << "] : " << bvolume[left]->max.x << "  "
      << "right[" << right << "] : "  << bvolume[right]->max.x << endl;
    */

    while(left < right){
        if(f % 3 == 0){
            while(left < e-1 &&
                  bvolume[left]->get_gravity_center().x < pivot)
                left++;
        }
        else if(f % 3 == 1){
            while(left < e-1 &&
                  bvolume[left]->get_gravity_center().y < pivot)
                left++;
        }
        else{
            while(left < e-1 &&
                  bvolume[left]->get_gravity_center().z < pivot)
                left++;
        }


        if(f % 3 == 0){
            while(right >= s+1 &&
                  bvolume[right]->get_gravity_center().x >= pivot)
                right--;
        }
        else if(f % 3 == 1){
            while(right >= s+1 &&
                  bvolume[right]->get_gravity_center().y >= pivot)
                right--;
        }
        else{
            while(right >= s+1 &&
                  bvolume[right]->get_gravity_center().z >= pivot)
                right--;
        }

        /*
          while((left < e-1 && bvolume[left]->max.x < pivot && f % 3 == 0) ||
          (left < e-1 && bvolume[left]->max.y < pivot && f % 3 == 1) ||
          (left < e-1 && bvolume[left]->max.z < pivot && f % 3 == 2))
          left++;
          while((right >= s + 1 && bvolume[right]->max.x >= pivot && f % 3 == 0) ||
          (right >= s + 1 && bvolume[right]->max.y >= pivot && f % 3 == 1) ||
          (right >= s + 1 && bvolume[right]->max.z >= pivot && f % 3 == 2))
          right--;
        */
        if(left >= right) break;
        t = bvolume[left];
        bvolume[left] = bvolume[right];
        bvolume[right] = t;
        left++;
        right--;
    }

    // cout << "return : " << left << endl;
    return left;
}

void Scene::qsort(int s, int e, int f){
    int k;

    // sからe-1まで
    // cout << "qsort[" << "] (" << s << ", " << e << ")" << endl;

    if(s == e) return;
    k = partition(s, e, f);

    // cout << "qsort[" << f + 1 << "] (" << s << ", " << k << ")" << endl;
    qsort(s, k, f);
    //  cout << "qsort[" << f + 1 << "] (" << k << ", " << e << ")" << endl;
    qsort(k+1, e, f);
}

AABB3* Scene::division(int s, int e, int f){
    int i, d, n;
    AABB3* ret;
    bool x;

    x = false;
    ret = new AABB3();

    ret->empty();
    ret->node = NULL;

    d = e - s;

    qsort(s, e, f);

    if(d == 2 || d == 1){
        // s , s+1
        ret->right = bvolume[s];
        if(d == 2){
            ret->left  = bvolume[s+1];
        }
    }
    else{
        for(i=s;i<e;i++){
            ret->add(bvolume[i]->max);
            ret->add(bvolume[i]->min);
        }
        n = (d / 2) + s;
        // cout << "R(" << s << ", " << n << ")" << endl;
        ret->right = division(s, n, f+1);
        // cout << "L(" << n << ", " << e << ")" << endl;
        ret->left  = division(n, e, f+1);
    }

    return ret;
};

void Scene::build_bounding_box(){
    int i;
    AABB3 v;
    vector<AABB3*> ba;
    
    bvolume.reserve(model.size());
    
    // 最も小さなバウンディングボックスを生成する(ノード付き)
#ifdef _OPENMP
#pragma omp parallel for private(i)
#endif
    for(i=0;i<(int)model.size();i++){
        bvolume[i] = new AABB3();
        bvolume[i]->empty();
        bvolume[i]->add(model[i]->get_max());
        bvolume[i]->add(model[i]->get_min());
        bvolume[i]->node  = model[i];
        bvolume[i]->right = NULL;
        bvolume[i]->left  = NULL;
    }
    
    bbtree = division(0, (int)model.size(), 0);
};

void Scene::building_tree(int s, int e, int f, int idx){
    int i;
    int num;

    num = e - s;

    /*
    cout << "Sort["<< idx << "]"
         << "[" << s << "," << e << ") at " << f << " : " << num << endl;
         */

    btree[idx].empty();
    
    qsort(s, e, f); // bvolume[i]のsからe-1までをフラグfでソート
    for(i=s;i<e;i++){
        btree[idx].add(bvolume[i]->max);
        btree[idx].add(bvolume[i]->min);
    }
    //btree[idx].max.show();
    //btree[idx].min.show();

    if(num > child_num){
        /*
        building_tree(s,      (s+e)/2, f+1, idx*2+1);
        building_tree((s+e)/2, e,      f+1, idx*2+2);
        */
        /*
        cout << " Dvidison : " << s << ", "
             << s+num*1/4 << ", " << s+num*2/4 << ", "
             << s+num*3/4 << ", " << e << endl;
        building_tree(s,         s+num*1/4, f+1, idx*4+1);
        building_tree(s+num*1/4, s+num*2/4, f+1, idx*4+2);
        building_tree(s+num*2/4, s+num*3/4, f+1, idx*4+3);
        building_tree(s+num*3/4, e,         f+1, idx*4+4);
        */
        for(i=0;i<child_num;i++){
            if(i+1 != child_num){
                building_tree(s+num*i/child_num,
                              s+num*(i+1)/child_num,
                              f+1,
                              idx*child_num+(i+1));
            }
            else{
                building_tree(s+num*i/child_num,
                              e,
                              f+1,
                              idx*child_num+(i+1));
            }
        }
    }
    else{
        btree[idx].model.resize(num);
        // cout << "  Reserve " << btree[idx].model.size() << " nodes." << endl;
        for(i=0;i<num;i++){
            btree[idx].model[i] = bvolume[s+i]->node;
            /*
            cout << "btree[" << idx << "].model[" << i << "]"
                 << ": bvolume[" << s+i << "]" << endl;
                 */
        }
    }
}

void Scene::build_bounding_box2(){
    int i;
    AABB3 v;
    vector<AABB3*> ba;

    int depth;
    int node_num;

    bvolume.reserve(model.size());

    // 最も小さなバウンディングボックスを生成する(ノード付き)
#ifdef _OPENMP
#pragma omp parallel for private(i)
#endif
    for(i=0;i<(int)model.size();i++){
        bvolume[i] = new AABB3();
        bvolume[i]->empty();
        bvolume[i]->add(model[i]->get_max());
        bvolume[i]->add(model[i]->get_min());
        bvolume[i]->node  = model[i];
        bvolume[i]->right = NULL;
        bvolume[i]->left  = NULL;
    }

    child_num = 2;
    
    depth = log2(model.size()) / log2(child_num);
    /*
      cout << "Model Size: " << model.size() << endl;
      cout << "Tree Depth: " << depth
      << " (" << (int)pow((double)child_num, depth)<< ")" << endl;

      cout << "Tree Size: " <<  (int)pow((double)child_num, depth-1)
      << " + " << model.size() << endl;
    */

    //node_num = (int)pow((REAL)child_num, depth + 1) - 1;
    node_num = 0;
    for(i=0;i<=depth;i++){
        // cout << i << " : " << node_num << " : ";
        node_num += (int)pow((REAL)child_num, i);
        // cout << (int)pow((REAL)child_num, i) << endl;
    }

    cout << "Tree Node Number: " << node_num
         << " (" << child_num << "**" << depth << ")"<< endl;
    
    btree.resize(node_num);
    
    int s, e, f;
    int idx;

    f = 0; idx = 0; s = 0; e = model.size();
    building_tree(s, e, f, idx);

    cout << "end of building_tree" << endl;
    
    // bbtree = division(0, (int)model.size(), 0);
};



void Scene::load_ply(string filename){
    int elnum;
    int N = 256;
    FILE* fp;
    char line[N];
    vector<string> svector(5);
    int header_flag = 1;
    REAL vx, vy, vz;
    Vector3 v;
    int p1, p2, p3;
    vector<Vector3> vertex_list;
    int vertex_num, face_num;

    face   = 0;
    vertex = 0;

    fp = fopen(filename.c_str(), "r");
    if(fp == NULL){
        printf("%sファイルが開けません\n", filename.c_str());
        exit(-1);
    }

    while (fgets(line, N, fp)!= NULL) {
        line[strlen(line) - 1] = '\0'; // 改行コード削除
        elnum = Scene::split2(&svector, line, string(" "));
        
        if(header_flag == 1){
            if(svector[0] == "comment"){
                // cout << "cmt" << endl;
            }
            else if(svector[0] == "element"){
                if(svector[1] == "vertex"){
                    vertex = atoi(svector[2].c_str());
                    vertex_num = vertex;
                    vertex_list.resize(vertex);
                }
                else if(svector[1] == "face"){
                    face = atoi(svector[2].c_str());
                    face_num = face;
                    model.resize(face);
                }
            }
            else if(svector[0] == "end_header"){
                // cout << "end_header" << endl;
                header_flag = 2;
            }
        }
        else if(header_flag == 2){
            if(vertex_num != 0){
                vx = (REAL)atof(svector[0].c_str());
                vy = (REAL)atof(svector[1].c_str());
                vz = (REAL)atof(svector[2].c_str());
                vertex_list[vertex-vertex_num].set_vector(vx, vy, vz);
                vertex_num--;
            }
            else if(face_num != 0){
                p1 = atoi(svector[1].c_str());
                p2 = atoi(svector[2].c_str());
                p3 = atoi(svector[3].c_str());
                model[face-face_num] = new Polygon3(//new Color(rand()%256,rand()%256,rand()%256),
                                                    //new Color(255,255,0),
                    new Color(255,255,255),
                    // new Color(rand()%256,rand()%256,rand()%256),
                    &vertex_list[p1],
                    &vertex_list[p2],
                    &vertex_list[p3]);
                face_num--;
            }
        }
    }

    fclose(fp);
}

char* Scene::getValue(char* src, const char* key){
    int i;
    char *ps, *pe;
    char *val;

    ps = strstr(src, key);
    if(ps == NULL){
    }
    ps += strlen(key);
    pe = strchr(ps, (char)0x0D);

    val = (char*)malloc(sizeof(char)*(pe-ps));

    //printf("len: %d\n", pe - ps);
    for(i=0;i<(pe-ps);i++){
        val[i] = ps[i];
    }

    return val;
}

void Scene::convertEndian(char* array, int size){
    char tmp;
    char *head, *tail;

    head = array;
    tail = array + size - 1;

    while(head < tail){
        tmp   = *head;
        *head = *tail;
        *tail = tmp;
        head++;
        tail--;
    }
}

void Scene::load_binary_ply(string filename){
    int i;
    char *c;
    int file_size;
    ifstream ifs;
    char *header, *body;
    int unit_size;
    vector<Vector3> vertex_list;
    char *head, *p;
    
    ifs.open(filename.c_str(), ios::in | ios::binary);
    if(ifs.fail()){
        cerr << "File do not exist." << endl;
        exit(1);
    }
    
    ifs.seekg(0, ios::end);
    file_size = ifs.tellg();
    ifs.seekg(0, ios::beg);
    
    c = (char*)malloc(sizeof(char)*file_size);
    ifs.read((char*)c, sizeof(char)*file_size);
    
    header = c;
    p = strstr(c, "end_header\r");
    body = p + strlen("end_header\r");
    //ifs.close();

    
    vertex = atoi(getValue(header, "element vertex "));
    //printf("vertex_num : %d\n", vertex_num);
    face = atoi(getValue(header, "element face "));
    //printf("face_num   : %d\n", face_num);
    
    vertex_list.resize(vertex);
    model.resize(face);
    
    
    head = body;
    unit_size = sizeof(float) * 3;
    for(i=0;i<vertex;i++){
        convertEndian(body+unit_size*i+0, sizeof(float));
        convertEndian(body+unit_size*i+4, sizeof(float));
        convertEndian(body+unit_size*i+8, sizeof(float));
        /*
        printf("%8.6f ", *(float*)(body+unit_size*i+0));
        printf("%8.6f ", *(float*)(body+unit_size*i+4));
        printf("%8.6f\n", *(float*)(body+unit_size*i+8));
        */
        vertex_list[i].set_vector(*(float*)(body+unit_size*i+0),
                                  *(float*)(body+unit_size*i+4),
                                  *(float*)(body+unit_size*i+8));
    }
    
    head = body + sizeof(float) * vertex * 3;
    unit_size = sizeof(unsigned char)+3*sizeof(unsigned int);    
    for(i=0;i<face;i++){
        // printf("%02X ", *(unsigned char*)head);
        convertEndian((head+unit_size*i+1), sizeof(unsigned int));
        convertEndian((head+unit_size*i+5), sizeof(unsigned int));
        convertEndian((head+unit_size*i+9), sizeof(unsigned int));
        /*
        printf("%08X ", *(unsigned int*)(head+unit_size*i+1));
        printf("%08X ", *(unsigned int*)(head+unit_size*i+5));
        printf("%08X\n", *(unsigned int*)(head+unit_size*i+9));
        */
        model[i] = new Polygon3(new Color(255,255,255),
                                &vertex_list[*(unsigned int*)(head+unit_size*i+1)],
                                &vertex_list[*(unsigned int*)(head+unit_size*i+5)],
                                &vertex_list[*(unsigned int*)(head+unit_size*i+9)]);
    }
}


/*
  void Scene::load_ply(string filename){
  ifstream ifs;
  string s;
  vector<string> slist;
  int header_flag = 1;
  REAL vx, vy, vz;
  Vector3 v;
  int p1, p2, p3;
  vector<Vector3> vertex_list;
  int vertex_num, face_num;

  face   = 0;
  vertex = 0;

  ifs.open(filename.c_str());
  while(getline(ifs, s)){
  slist = Scene::split(s, string(" "));

  if(header_flag == 1){
  if(slist[0] == "comment"){
  // cout << "cmt" << endl;
  }
  else if(slist[0] == "element"){
  if(slist[1] == "vertex"){
  vertex = atoi(slist[2].c_str());
  vertex_num = vertex;
  }
  else if(slist[1] == "face"){
  face = atoi(slist[2].c_str());
  face_num = face;
  }
  }
  else if(slist[0] == "end_header"){
  header_flag = 2;
  }
  }
  else if(header_flag == 2){
  if(vertex_num != 0){
  vertex_num--;
  vx = (REAL)atof(slist[0].c_str());
  vy = (REAL)atof(slist[1].c_str());
  vz = (REAL)atof(slist[2].c_str());
  v.set_vector(vx, vy, vz);
  vertex_list.push_back(v);
  }
  else if(face_num != 0){
  face_num--;
  p1 = atoi(slist[1].c_str());
  p2 = atoi(slist[2].c_str());
  p3 = atoi(slist[3].c_str());

  model.push_back(new Polygon3(//new Color(rand()%256,rand()%256,rand()%256),
  //new Color(255,255,0),
  new Color(255,255,255),
  // new Color(rand()%256,rand()%256,rand()%256),
  &vertex_list[p1],
  &vertex_list[p2],
  &vertex_list[p3]));
  }
  }
  }
  //  cout << vertex << face << endl;
  ifs.close();
  }
*/


// view : スクリーンの画素位置へのベクトル
Color Scene::intersect(Vector3 view){
    int m;
    REAL dv, t;
    Vector3 view_vector; // 視線(単位)ベクトル
    vector<pair<Polygon3*, double> > xlist; // 交差したモデルのリスト
    Color c;
    Polygon3* min_id;
    REAL min;
    Vector3 tv, np, n, light_v;
    list<Polygon3*> mlist;
    Polygon3* tm;

    // 視線(単位)ベクトルの計算
    dv = sqrt(view.x * view.x + view.y * view.y + view.z * view.z);
    view_vector.set_vector(viewpoint.x + view.x,
                           viewpoint.y + view.y,
                           viewpoint.z + (1.0 * dv));
    view_vector = view_vector - viewpoint;
    view_vector.normalize();

    // バウンディングボックスの探索
    // ボックスの交差判定
    // tree[0].node.x
    xlist.empty();

#ifdef BVH_LIST
    mlist = tree_traversal(view_vector);
#else
    mlist = tree_traversal2(view_vector);
#endif

    // cout << "Polygon3 intersect : " << (int)mlist.size() << endl;
    // 交差判定
    for(m = 0; m<(int)mlist.size(); m++){
        tm = mlist.front();
        t = tm->intersect(viewpoint, view_vector);
        if(t < HUGE_VAL){
            xlist.push_back(pair<Polygon3*, double>(tm, t));
        }
        mlist.pop_front();
        mlist.push_back(tm);
    }

#ifdef _OPENMP
    omp_set_lock(&lock_pnum);
    pnum+=(int)mlist.size();
    omp_unset_lock(&lock_pnum);
#else
    pnum+=(int)mlist.size();
#endif

    
    if(xlist.size() == 0){
        c.set_color(bgcolor.red,
                    bgcolor.green,
                    bgcolor.blue);
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
        np = viewpoint + tv;
        // 物体の法線ベクトル(単位ベクトル)の計算
        // n  = scene->model[min_id]->get_normal_vector(np);
        n  = min_id->get_normal_vector(np);
        // 光線ベクトル
        light_v = np - light[0]->get_vector();
        light_v.normalize();
        c = shading(view_vector,
                    light_v, n, min_id->get_color(),
                    0.5);
    }
    return c;
};

Color Scene::shading(Vector3 view_vector,
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

