#include<iostream>
#include<stdio.h>
#include<string.h>
#include<vector>
#include<string>

using namespace std;

int main(int argc, char** argv){
    int i, j;
    int N = 256;
    FILE *fp;
    char* fname = (char*)"./test.txt";
    // char* fname = (char*)"./happy_vrip_res4.ply";
    char line[N];
    char *tp;
    vector<string> svector(5);

    const char *delim = " ";
    
    fp = fopen(fname, "r");
    if(fp == NULL){
        printf("%sファイルが開けません\n", fname);
        return -1;
    }
    
    while (fgets(line, N, fp)!= NULL) {
        line[strlen(line) - 1] = '\0'; // 改行コード削除
        i = 0;
        tp = strtok(line, delim);
        svector[i] = string(tp);
        while(tp != NULL){
            tp = strtok(NULL, delim);
            if(tp != NULL){
                i++;
                svector[i] = string(tp);
                // svector.push_back(string(tp));
            }
        }
        for(j=0;j<=i;j++){
            printf("%s+", svector[j].c_str());
        }
        printf("\n");
    }

    
    
    fclose(fp);
    
    return 0;
}

