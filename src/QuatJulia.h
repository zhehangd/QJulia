// File:   QuatJulia.cpp
// Author: Zhehang Ding
// Email:  dingzhehang1993@gmail.com
// Data:   Feb. 08, 2016

#ifndef QUATJULIA_H_
#define QUATJULIA_H_

#include <vector>
#include "Image.h"
#include "Math.h"

struct qSurfaceGeneratorParm
{
    // Canvas information
    int width;
    int height;
    // Fractal information
    int   thres;
    int   div;
    float preci;
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


Image qSurfaceGenerator(const qSurfaceGeneratorParm &parm, const Camera &cam);

Image qComputeNormal(Image surf);

Image qLighting(Image surf,Image norm,qLightFieldParm &parm);



#endif
