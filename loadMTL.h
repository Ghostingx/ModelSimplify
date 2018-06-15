#pragma once
#ifndef LOADMTL_H
#define LOADMTL_H
#include<string.h>
#include<stdio.h>
#include<vector>

struct Material{
public:
    char name[16];
    int Ns;
    float Ni;
    float d;
    int Tf[3];
    int illum;
    float Ka[3];
    float Kd[3];
    float Ks[3];
    char map_Kd[32];
};
int loadMTL(std::vector<Material*> &m,const char *file);
#endif
