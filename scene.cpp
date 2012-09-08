#include<vector>
#include<list>
#include<fstream>

#include "443ray.hpp"
#include "scene.hpp"


void Scene::add_light(Light* m){
  light.push_back(m);
};

void Scene::set_bgcolor(UINT8 r, UINT8 g, UINT8 b){
  bgcolor.red   = r;
  bgcolor.green = g;
  bgcolor.blue  = b;
};

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

list<Polygon3*> Scene::tree_traversal(Vector3 view_vector){
  AABB3* tree_ptr;
  list<AABB3*> traverse_list; // トラバース対象ノードのリスト
  list<Polygon3*> model_list; // 交差判定対象モデルのリスト

  tree_ptr = bbtree;

  traverse_list.clear();
  model_list.clear();

  num++;
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
        num++;
        if(tree_ptr->right->intersect(viewpoint, view_vector)){
          // cout << "add right" << endl;
          traverse_list.push_back(tree_ptr->right);
        }
      }

      if(tree_ptr->left  != NULL){
        // 左の子と交差判定
        num++;
        if(tree_ptr->left->intersect(viewpoint, view_vector)){
          // cout << "add left" << endl;
          traverse_list.push_back(tree_ptr->left);
        }
      }
    }
  }

  // cout << "box intersect : " << num << endl;

  return model_list;
}



int Scene::partition(int s, int e, int f){
  int right, left;
  REAL pivot;
  AABB3* t;

  /*
  if(f % 3 == 0){
    pivot = bvolume[s]->max.x;
  }
  else if(f % 3 == 1){
    pivot = bvolume[s]->max.y;
  }
  else{
    pivot = bvolume[s]->max.z;
  }
  */

  if(f % 3 == 0){
    pivot = (bvolume[s]->max.x + bvolume[e-1]->max.x) / 2.0;
  }
  else if(f % 3 == 1){
    pivot = (bvolume[s]->max.y + bvolume[e-1]->max.y) / 2.0;
  }
  else{
    pivot = (bvolume[s]->max.z + bvolume[e-1]->max.z) / 2.0;
  }

  //pivot = bvolume[s]->max.x;

  left  = s;
  right = e - 1;
  /*
  cout << "partition" << "(" << s << ", " << e << ")" << endl;
  cout << "   pivot[" << s << "] : " << pivot << endl;
  cout << "     left[" << left << "] : " << bvolume[left]->max.x << "  "
       << "right[" << right << "] : "  << bvolume[right]->max.x << endl;
  */

  while(left < right){
    while((left < e-1 && bvolume[left]->max.x < pivot && f % 3 == 0) ||
          (left < e-1 && bvolume[left]->max.y < pivot && f % 3 == 1) ||
          (left < e-1 && bvolume[left]->max.z < pivot && f % 3 == 2))
      // while(left < e-1 && bvolume[left]->max.x < pivot)
      left++;
    while((right >= s + 1 && bvolume[right]->max.x >= pivot && f % 3 == 0) ||
          (right >= s + 1 && bvolume[right]->max.y >= pivot && f % 3 == 1) ||
          (right >= s + 1 && bvolume[right]->max.z >= pivot && f % 3 == 2))
      // while(right >= s + 1 && bvolume[right]->max.x >= pivot)
      right--;
    // cout << "     left[" << left << "] : " << bvolume[left]->max.x << "  "
    // << "right[" << right << "] : "  << bvolume[right]->max.x << endl;

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
  // qsort(s, k, f+1);
  qsort(s, k, f);
  //  cout << "qsort[" << f + 1 << "] (" << k << ", " << e << ")" << endl;
  // qsort(k+1, e, f+1);
  qsort(k+1, e, f);
}


AABB3* Scene::division(int s, int e, int f){
  int i, j, d, n;
  AABB3* ret;
  AABB3* t;
  bool x;

  x = false;
  ret = new AABB3();

  ret->empty();
  ret->node = NULL;

  d = e - s;

  qsort(s, e, f);

  // バブルソート
  /*
  for(i=s;i<e;i++){
    for(j=i+1;j<e;j++){
      if(f % 3 == 0) x = (bvolume[j-1]->max.x > bvolume[j]->max.x);
      if(f % 3 == 1) x = (bvolume[j-1]->max.y > bvolume[j]->max.y);
      if(f % 3 == 2) x = (bvolume[j-1]->max.z > bvolume[j]->max.z);

      if(x){
        t = bvolume[j];
        bvolume[j] = bvolume[j-1];
        bvolume[j-1] = t;
      }
    }
  }
  */

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
  for(i=0;i<(int)model.size();i++){
    v.empty();
    v.add(model[i]->get_max());
    v.add(model[i]->get_min());
    v.node  = model[i];
    v.right = NULL;
    v.left  = NULL;
    bvolume[i] = v.clone();
  }

  bbtree = division(0, (int)model.size(), 0);
};

void Scene::load_ply(string filename){
  ifstream ifs;
  string s;
  list<string> slist;
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
      if(slist.front() == "comment"){
        // cout << "cmt" << endl;
      }
      else if(slist.front() == "element"){
        slist.pop_front();
        if(slist.front() == "vertex"){
          slist.pop_front();
          vertex = atoi(slist.front().c_str());
          vertex_num = vertex;
          // cout << "vertex : " << vertex << endl;
        }
        else if(slist.front() == "face"){
          slist.pop_front();
          face = atoi(slist.front().c_str());
          face_num = face;
          // cout << "face : " << face << endl;
        }
      }
      else if(slist.front() == "end_header"){
        header_flag = 2;
      }
    }
    else if(header_flag == 2){
      if(vertex_num != 0){
        vertex_num--;
        vx = (REAL)atof(slist.front().c_str());
        slist.pop_front();
        vy = (REAL)atof(slist.front().c_str());
        slist.pop_front();
        vz = (REAL)atof(slist.front().c_str());
        slist.pop_front();
        v.set_vector(vx, vy, vz);
        vertex_list.push_back(v);
      }
      else if(face_num != 0){
        face_num--;
        slist.pop_front();
        p1 = atoi(slist.front().c_str());
        slist.pop_front();
        p2 = atoi(slist.front().c_str());
        slist.pop_front();
        p3 = atoi(slist.front().c_str());
        slist.pop_front();

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
