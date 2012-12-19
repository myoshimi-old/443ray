#include<iostream>
#include<string>
#include<stdlib.h>
#include<fstream>

using namespace std;

char* getValue(char* src, const char* key){
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

void convertEndian(char* array, int size){
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

int main(int argc, char** argv){
    ifstream ifs("./BBMbunny.ply", ios::in | ios::binary);
    if(ifs.fail()){
        cerr << "File do not exist." << endl;
        return 1;
    }
    
    int i,j;
    string str;
    char *c;

    ifs.seekg(0, ios::end);
    int file_size = ifs.tellg();
    ifs.seekg(0, ios::beg);
    
    i = 0;

    c = (char*)malloc(sizeof(char)*file_size);
    ifs.read((char*)c, sizeof(char)*file_size);

    /*
    for(i=0;i<file_size;i++){
        printf("%02X", (unsigned char)c[i]);
    }
    printf("\n");
    */

    int status = 0;
    char* p;
    unsigned int n;
    cout << file_size << endl;
    ifs.close();

    char *header, *body;
    int header_size, body_size;
    
    header = c;
    p = strstr(c, "end_header\r");

    /*
    for(i=0;i<(p-c);i++){
        if(c[i] == '\r'){
            printf("\n");
        }
        printf("%c", c[i]);
    }
    */

    body = p + strlen("end_header\r");
    
    char *ps, *pe;
    int vertex_num, face_num;
    char *ppp;
    char *head;

    ppp = getValue(header, "element vertex ");
    vertex_num = atoi(ppp);
    printf("vertex_num : %d\n", vertex_num);

    ppp = getValue(header, "element face ");
    face_num = atoi(ppp);
    printf("face_num   : %d\n", face_num);

    printf("%08X\n", *(unsigned int*)body);
    
    int unit_size;
    head = body;
    unit_size = sizeof(float) * 3;
    for(i=0;i<vertex_num;i++){
        convertEndian(body+unit_size*i+0, sizeof(float));
        convertEndian(body+unit_size*i+4, sizeof(float));
        convertEndian(body+unit_size*i+8, sizeof(float));

        printf("%8.6f ", *(float*)(body+unit_size*i+0));
        printf("%8.6f ", *(float*)(body+unit_size*i+4));
        printf("%8.6f\n", *(float*)(body+unit_size*i+8));
    }

    head = body + sizeof(float) * vertex_num * 3;
    unit_size = sizeof(unsigned char)+3*sizeof(unsigned int);

    for(i=0;i<face_num;i++){
        printf("%02X ", *(unsigned char*)head);
        convertEndian((head+unit_size*i+1), sizeof(unsigned int));
        convertEndian((head+unit_size*i+5), sizeof(unsigned int));
        convertEndian((head+unit_size*i+9), sizeof(unsigned int));
        printf("%08X ", *(unsigned int*)(head+unit_size*i+1));
        printf("%08X ", *(unsigned int*)(head+unit_size*i+5));
        printf("%08X\n", *(unsigned int*)(head+unit_size*i+9));
    }


    return 0;
}
