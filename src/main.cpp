// File:  main.cpp
// Name:  Zhehang Ding
// USCID: 7543417727
// Email: zhehangd@usc.edu
// Data:  Nov. 29, 2015

#include <cassert>
#include <ctime>

#include <iostream>
#include <string>

#include "Image.h"
#include "QuatJulia.h"

using namespace std;


// Arugment
// -s [width height]   Image Width
// -f [focus=0.7]   Focus
// -t [thres=100]   Threshold
// -p [preci=0.0001] Precision
// -z [zmin zmax]   
// -q [a b c d]
// -v [h v]
// -o  [canv]
// -os [surf]
// -on [normal]
// -la [r g b]
// -l0a [att]
// -l0p
// -l0v 
// -l1                 Use l1
// -l2                 Use l2
// -l1/2apv
// -i [config file]
// -console            console mode
// -h                  help
int main(int argc,char **argv)
{
    
    // Set shown float precision to 3.
    std::cout.precision(3);

    // Timer begins.
    std::clock_t counter = std::clock();
    
    // =========================
    
    {      
        //ostringstream ss;
        //ss << "output/step_"<<std::setfill('0')<<std::setw(2)<<i<<".data";
        
        //std::cout<<"#"<<i<<std::endl;
        qSurfaceGeneratorParm parm1;
        parm1.width  = 1600;
        parm1.height = 1600;
        parm1.focus  = 1.2;
        parm1.thres  = 100;
        parm1.preci  = 0.0001;
        parm1.zmin   = 0;
        parm1.zmax   = 5;
        parm1.qc     = Quaternion(-0.2,-0.8,0.0,0.0); // The classic one
        //parm1.qc     = Quaternion(0.3,0.2,0.6,0.5);
        
        qCameraParm camp;
        camp.setup(180+50,20);
        //camp.setup(Vector3(1,1,1),Vector3(0,0,0));
        //camp.setup(Vector3(-0.3,0.4,1),Vector3(0,0.6,0)); // loop focus=2
        //camp.setup(Vector3(-2,1,2),Vector3(-1.1,-0.5,0)); // head focus=5
        
        
        cout<<"Iteratively calculate the surface..."<<endl;
        
        // Find surface of the Julia set.
        Image surf = qSurfaceGenerator(parm1,camp);
        
        ((surf + 1.0f)*128).convert(IMAGE_CHAR).write("output/Julia_surf.data");
        
        cout<<"Calculating normal vectors..."<<endl;
        
        // Compute the normal of each pixel.
        Image norm  = qComputeNormal(surf);
        
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
        
        
        cout<<"Shading the surface..."<<endl;
        
        Image canv = qLighting(surf,norm,parm2);
        
        cout<<"Complete."<<endl;
        
        
        (norm*256).convert(IMAGE_CHAR).write("output/Julia_norm.data");
        (canv*256).convert(IMAGE_CHAR).write("output/Julia_canv.data");
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
