// File:   main.cpp
// Author: Zhehang Ding
// Email:  dingzhehang1993@gmail.com
// Data:   Feb. 08, 2016

#include <cassert>
#include <ctime>

#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>

#include "Parser.h"
#include "Image.h"
#include "QuatJulia.h"

using namespace std;

// This is still in the very begining.
// Apologies for all the mess of code.

Image draw(qSurfaceGeneratorParm &parm1,qLightFieldParm &parm2,qCameraParm &camp)
{

    cout<<"Calculating the surface..."<<endl;
    Image surf = qSurfaceGenerator(parm1,camp);
    
    cout<<"Calculating the normal vectors..."<<endl;
    Image norm  = qComputeNormal(surf);
    
    cout<<"Shading the surface..."<<endl;
    Image canv = qLighting(surf,norm,parm2);
    
    ((surf + 1.0f)*128).convert(IMAGE_CHAR).write("Julia_surf.data");
    (norm*256).convert(IMAGE_CHAR).write("Julia_norm.data");
    (canv*256).convert(IMAGE_CHAR).write("Julia_canv.data");
    
    //cout<<"Complete."<<endl;
    
    return (canv*256).convert(IMAGE_CHAR);
    
}

// -s size                 ~
// -f fov                  ~
// -d div
// -t threshold
// -p precision
// -z "zmin zmax"
// -q "a i j k"            ~
// -c "h v"                ~
// -C "ex ey ez tx ty tz"  ~
// -l a
// -l 0/1/2a/d/pr,g,b
// -o filename
int main(int argc,const char **argv)
{
    
    // Set shown float precision to 3.
    std::cout.precision(3);

    // Timer begins.
    std::clock_t counter = std::clock();
    
    // =========================
    
    {
//  ==================== Set the initial parameters  ====================
        
        // Param - Rendering
        qSurfaceGeneratorParm parm1;
        parm1.width  = 4*512;
        parm1.height = 4*512;
        parm1.fov    = 0.7;
        parm1.div    = 100;
        parm1.thres  = 16;
        parm1.preci  = 0.0001;
        parm1.zmin   = 0;
        parm1.zmax   = 5;
        parm1.qc     = Quaternion(-0.2,-0.8,0.0,0.0);
        

        // Parm - Camera
        qCameraParm camp;
        camp.setup(60+180,30);
        //camp.setup(Vector3(-1.2555,0.342,0.5),Vector3(-1.2555,0.342,0));
        
        
        // Parm - Lighting
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
        
        
        
        
//  ======================== Parse the arguments  ========================
        
        
        // Initialize the parser.
        Parser &parser = Parser::getInstance();
        parser.add('s',true); parser.add('f',true);
        parser.add('d',true); parser.add('t',true);
        parser.add('p',true); parser.add('z',true);
        parser.add('q',true); parser.add('c',true);
        parser.add('l',true); parser.add('o',true);
        parser.add('C',true);
        // Parse the arguments.
        parser.parse(argc,argv);
        
        for(auto opt : parser.input)
        {
            cout<<(opt.first==0?'-':opt.first)<<":  "<<opt.second<<endl;
            //
            bool good = true;
            // Set up the stream.
            istringstream ss(opt.second);
            switch(opt.first)
            {
                case 's':
                    ss>>parm1.width>>parm1.height;break;
                case 'f':
                    ss>>parm1.fov;break;
                case 'q':
                    ss>>parm1.qc.a>>parm1.qc.i>>parm1.qc.j>>parm1.qc.k;break;
                case 'c':{ float h=0,v=0;
                    ss>>h>>v; camp.setup(h,v);break;}
                case 'C':{float f[3],t[3];
                    ss>>f[0]>>f[1]>>f[2]>>t[0]>>t[1]>>t[2];
                    camp.setup(Vector3(f),Vector3(t));break;}
                     
            }
            good = good && !ss.fail();
            if(good==false)
            {
                cout<<"Syntax Error."<<endl;
                exit(-1);
            }
        
        }
        
        std::cout<<parm1.width<<" - "<<parm1.height<<endl;
        std::cout<<parm1.qc.a<<" "<<parm1.qc.i<<" "<<parm1.qc.j<<" "<<parm1.qc.k<<endl;
        
        
        
        


        Image canvas = draw(parm1,parm2,camp);
        //canvas.write("output.data");
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
