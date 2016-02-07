// File:   main.cpp
// Author: Zhehang Ding
// Email:  dingzhehang1993@gmail.com
// Data:   Feb. 07, 2016

#include <cassert>
#include <ctime>

#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>

#include "Image.h"
#include "QuatJulia.h"

using namespace std;

// This is still in the very begining.
// Apologies for all the mess of code.

Image draw(qSurfaceGeneratorParm &parm1,qLightFieldParm &parm2,qCameraParm &camp)
{

    //cout<<"Iteratively calculate the surface..."<<endl;
    Image surf = qSurfaceGenerator(parm1,camp);
    
    //cout<<"Calculating normal vectors..."<<endl;
    Image norm  = qComputeNormal(surf);
    
    //cout<<"Shading the surface..."<<endl;
    Image canv = qLighting(surf,norm,parm2);
    
    //((surf + 1.0f)*128).convert(IMAGE_CHAR).write("output/Julia_surf.data");
    //(norm*256).convert(IMAGE_CHAR).write("output/Julia_norm.data");
    //(canv*256).convert(IMAGE_CHAR).write("output/Julia_canv.data");
    
    //cout<<"Complete."<<endl;
    
    return (canv*256).convert(IMAGE_CHAR);
    
}


int main(int argc,const char **argv)
{
    
    // Set shown float precision to 3.
    std::cout.precision(3);

    // Timer begins.
    std::clock_t counter = std::clock();
    
    // =========================
    
    {
        
        qSurfaceGeneratorParm parm1;
        parm1.width  = 512;
        parm1.height = 512;
        parm1.focus  = 0.7;
        parm1.thres  = 100;
        parm1.preci  = 0.000001;
        parm1.zmin   = -1;
        parm1.zmax   = 5;
        parm1.qc     = Quaternion(-0.2,-0.8,0.0,0.0); // The classic one
        //parm1.qc     = Quaternion(-0.3,0.5,-0.4,0.34);
        //parm1.qc     = Quaternion(0.3,0.2,0.6,0.5);
        

        qCameraParm camp;
        camp.setup(180+60,20);
        
        // Lighting.
        qLightParm light0;
        light0.att      = 5;
        light0.ambient  = Vector3(0,0.2,0.2);
        light0.diffuse  = Vector3(1,0.4,0.2)*5;
        light0.position = Vector3(-0.8,0.1,0.1);
        
        qLightParm light1;
        light1.att      = 5;
        light1.ambient  = Vector3(0,0,0);
        light1.diffuse  = Vector3(0.2,1,0.3)*3;
        light1.position = Vector3(1,1,0.2);
        
        qLightParm light2;
        light2.att      = 5;
        light2.ambient  = Vector3(0,0,0);
        light2.diffuse  = Vector3(0,0.4,1)*3;
        light2.position = Vector3(0,1,0);
        
        qLightFieldParm parm2;
        parm2.lights.push_back(light0);
        parm2.lights.push_back(light1);
        parm2.lights.push_back(light2);  


        Image canvas = draw(parm1,parm2,camp);
        canvas.write("output.data");
    }
    // =============================
    
    std::cout<<'\n';
    
    
    // Timer ends.
    float time = (std::clock() - counter) / (double) CLOCKS_PER_SEC;
    
    std::cout<<"Program complete."<<std::endl;
    std::cout<<"Timer: "<<time<<std::endl;

    std::cout<<std::endl;
    
    return 0;
}
