// File:   QuatJulia.cpp
// Author: Zhehang Ding
// Email:  dingzhehang1993@gmail.com
// Data:   Feb. 07, 2016

#ifndef QUATJULIA_H_
#define QUATJULIA_H_

#include <vector>
#include "Image.h"
#include "Math.h"

struct qCameraParm
{
    void setup(float h,float v);
    void setup(Vector3 at,Vector3 to);
    Vector3 pos;
    Vector3 dir;
};

struct qSurfaceGeneratorParm
{
    // Canvas information
    int width;
    int height;
    // Camera information
    float focus;
    // Fractal information
    int   thres;
    float preci;
    //
    float zmin;
    float zmax;
    //
    Quaternion qc;
};

struct qLightParm
{
    Vector3 ambient;
    Vector3 diffuse;
    Vector3 specular;
    Vector3 position;
    // Attenuation coefficient.
    float att;
};

struct qLightFieldParm
{
    std::vector<qLightParm> lights;
};


Image qSurfaceGenerator(const qSurfaceGeneratorParm &parm, const qCameraParm &camp);

Image qComputeNormal(Image surf);

Image qLighting(Image surf,Image norm,qLightFieldParm &parm);



#endif
