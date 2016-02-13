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

Image draw(qSurfaceGeneratorParm &parm1,qLightFieldParm &parm2,Camera &camera)
{

    cout<<"Calculating the surface..."<<endl;
    Image surf = qSurfaceGenerator(parm1,camera);
    
    cout<<"Calculating the normal vectors..."<<endl;
    Image norm  = qComputeNormal(surf);
    
    cout<<"Shading the surface..."<<endl;
    Image canv = qLighting(surf,norm,parm2);
    
    //((surf + 1.0f)*128).convert(IMAGE_CHAR).write("Julia_surf.data");
    //(norm*256).convert(IMAGE_CHAR).write("Julia_norm.data");
    //(canv*256).convert(IMAGE_CHAR).write("Julia_canv.data");
    
    //cout<<"Complete."<<endl;
    
    return (canv*256).convert(IMAGE_CHAR);
    
}

// -s size                 ~
// -f fov                  ~
// -d div                  ~
// -t threshold            ~
// -p precision            ~
// -z "zmin zmax"          ~
// -q "a i j k"            ~
// -v "h v"                ~
// -C "ex ey ez tx ty tz"  ~
// -c "ex ey ez h v"       ~
// -l a
// -l 0/1/2a/d/pr,g,b      ~
// -o filename             ~
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
        parm1.width  = 1024;
        parm1.height = 1024;
        parm1.div    = 100;
        parm1.thres  = 12;
        parm1.preci  = 0.0001;
        parm1.qc     = Quaternion(-0.2,0.8,0.0,0.0);

        
        Camera camera;
        camera.setupExt(60,30,1);
        camera.setupInt(1,0.3,10);
        
        
        // Parm - Lighting
        qLightParm light0; // Red
        light0.att      = 5;
        light0.ambient  = Vector3(0,0.2,0.2);
        light0.diffuse  = Vector3(1,0.4,0.2)*5;
        light0.position = Vector3(0.8,0.1,-0.1);
        
        qLightParm light1; // Green
        light1.att      = 5;
        light1.ambient  = Vector3(0,0,0);
        light1.diffuse  = Vector3(0.2,1,0.3)*3;
        light1.position = Vector3(-1,1,-0.2);
        
        qLightParm light2; // Blue
        light2.att      = 5;
        light2.ambient  = Vector3(0,0,0);
        light2.diffuse  = Vector3(0,0.4,1)*3;
        light2.position = Vector3(0,1,0);
        
        qLightFieldParm parm2;
        parm2.lights.push_back(light0);
        parm2.lights.push_back(light1);
        parm2.lights.push_back(light2);  
        
        // Output filename
        string filename = "Julia.data";
        
        
        
//  ======================== Parse the arguments  ========================
        
        
        // Initialize the parser.
        Parser &parser = Parser::getInstance();
        parser.add('s',true); parser.add('f',true);
        parser.add('d',true); parser.add('t',true);
        parser.add('p',true); parser.add('z',true);
        parser.add('q',true); parser.add('c',true);
        parser.add('l',true); parser.add('o',true);
        parser.add('C',true); parser.add('h',false);
        parser.add('v',true);
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
                case 'f':{float focus;
                    ss>>focus;
                    camera.setupInt(focus,camera.zn,camera.zf);break;}
                case 'q':
                    ss>>parm1.qc.a>>parm1.qc.i>>parm1.qc.j>>parm1.qc.k;break;
                case 'v':{ float h=0,v=0,r=1;
                    ss>>h>>v>>r; camera.setupExt(h,v,r);break;}
                case 'c':{float f[3],h,v;
                    ss>>f[0]>>f[1]>>f[2]>>h>>v;
                    camera.setupExt(Vector3(f),h,v);break;}
                case 'C':{float f[3],t[3];
                    ss>>f[0]>>f[1]>>f[2]>>t[0]>>t[1]>>t[2];
                    camera.setupExt(Vector3(f),Vector3(t));break;}
                case 't':
                    ss>>parm1.thres;break;
                case 'd':
                    ss>>parm1.div;break;
                case 'p':
                    ss>>parm1.preci;break;
                case 'z':{float zmin,zmax;
                    ss>>zmin>>zmax;
                    camera.setupInt(camera.f,zmin,zmax);break;}
                case 'o':
                    ss>>filename;break;
                case 'l':
                {
                    int id; char t; float r,g,b;
                    ss>>id>>t>>r>>g>>b;
                    assert(id<3);
                    switch(t){
                    case 'a': parm2.lights[id].ambient  = Vector3(r,g,b);break;
                    case 'd': parm2.lights[id].diffuse  = Vector3(r,g,b);break;
                    case 'p': parm2.lights[id].position = Vector3(r,g,b);break;
                    default:  assert(0);break;
                    }break;
                }
                case 'h': cout<<"sfqcCtoh\n"<<endl; exit(0); break;
            }
            good = good && !ss.fail();
            if(good==false)
            {
                cout<<"Syntax Error."<<endl;
                exit(-1);
            }
        
        }
        
        // Show all parameters
        
        cout<<"size:       "<<parm1.width<<" "<<parm1.height<<"\n";
        cout<<"fov:        "<<camera.f<<"\n";
        cout<<"div:        "<<parm1.div<<"\n";
        cout<<"thres:      "<<parm1.thres<<"\n";
        cout<<"preci:      "<<parm1.preci<<"\n";
        cout<<"zmin/max:   "<<camera.zn<<" "<<camera.zf<<"\n";
        cout<<"quaternion: "<<parm1.qc.a<<" "<<parm1.qc.i<<" "<<parm1.qc.j<<" "<<parm1.qc.k<<"\n";
        cout<<"camera:     "<<"N/A"<<"\n";
        cout<<"light:      "<<"N/A"<<"\n";
        cout<<"output:     "<<filename<<"\n";

        Image canvas = draw(parm1,parm2,camera);
        canvas.write(filename.c_str());
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
