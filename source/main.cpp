#include <iostream>
#include <iomanip>
#include <vector>
#include <memory>
#include <thread>
#include <complex>
#include <cmath>

#include "Console.hpp"
#include "Utility.hpp"
#include "Image.hpp"
#include "Perlin3d.hpp"
#include "Camera.hpp"
#include "Math.hpp"


# if 0

struct Light {
  Vector3 position;
  Vector3 ambient;
  Vector3 diffuse;
  Vector3 specular;
  float   shininess;
};

class App {
public:
  Camera  camera;
  Console console;
  std::vector<Light> lights;
  
  int width;
  int height;
  int nthreads;
  
  void start(void) {
    
    Image test(800,600,3,IMAGE_U8);
    std::cout<<"start"<<std::endl;
    ImCursor it(test,200,300,200,200);
    std::cout<<"start2"<<std::endl;
    for (int i=0; !it.eof(); i++) {
      it.pixel<unsigned char>(0) = 3 * (i)    % 255;
      it.pixel<unsigned char>(1) = 5 * (i+85) % 255;
      it.pixel<unsigned char>(2) = 7 * (i+85) % 255;
      it.moveNext();
      std::cout<<i<<" "<<it.cr<<" "<<it.cc<<std::endl;
    }
    
    imwrite(test,"test.ppm");
    
    
  }
  
} *app;

# endif





class App {
public:
  // Let us render a julia set,
  void start(void) {
    
    int width   = 800;
    int height  = 600;
    float cx    = 0.0f;
    float cy    = 0.0f;
    float focus = 0.004f;
    const int aa = 6;
    std::complex<float> c(-.835f,0);
    
    ImFloat* canvas = new ImFloat(width,height,3,IMAGE_F32);
    
    
    // x-shift, y-shift, weight
    float AAFilter[6][3] = {
      -.52f,  .38f, .128f, 
       .41f,  .56f, .119f,
       .27f,  .08f, .294f,
      -.17f, -.29f, .249f,
       .58f, -.55f, .104f,
      -.31f, -.71f, .106f,
    };
    
    CameraOrtho camera;
    camera.setupInt(1,1);
    camera.setupExt();
    camera.setupScn(width,height);
    

    Image* aabuff = new Image(width,height,aa,IMAGE_U8);
    
    for (int k=0;k<aa;k++) {
      for (ImCursor scan(*aabuff); !scan.eof(); scan.moveNext()) {
        
        float dst[3];
        float src[3] = {(float)scan.cc,(float)scan.cr,0};
        src[0] += AAFilter[k][0];
        src[1] += AAFilter[k][1];
        camera.projectInvXsw(dst,src);
        dst[0] += cx;
        dst[1] += cy;
        std::complex<float> x(dst[0],dst[1]);
        int val = 0;
        for(val=0;val<255;val++) {
          //x = (1+(n-1)*std::pow(x,n))/(n*std::pow(x,n-1));
          x = x*x + c;
          if (std::norm(x)>2.0f)
            break;
        }
        unsigned char* pix = (unsigned char*)scan.ptr();
        pix[k] = val;
      }
      
    }
    
    float max = 1e-6f;
    ImCursor scan_canvas(*canvas);
    ImCursor scan_buffer(*aabuff);
    for (;!scan_canvas.eof(); scan_canvas.moveNext(),scan_buffer.moveNext()) {
      float* val_canvas = (float*)scan_canvas.ptr();
      unsigned char* val_buffer = (unsigned char*)scan_buffer.ptr();
      
      float val = 0;
      for (int i=0;i<aa;i++)
        val += val_buffer[i];
      
      max  = val>max ? val : max;
      
      val_canvas[0] = (float) val;
      val_canvas[1] = (float) val;
      val_canvas[2] = (float) val;
    }
    
    for (ImCursor scan(*canvas); !scan.eof(); scan.moveNext()) {
      float* val = (float*)scan.ptr();
      val[0] /= max;
      val[1] /= max;
      val[2] /= max;
    }
    
    
    imwrite(*canvas,"test.ppm");
    
    
  }
  
  
};














int main(int argc,const char **argv) {
  
  
  App* app = new App();
  app->start();
  
  return 0;
  
}



/*
set_system nthreads 4
set_system filename julia.ppm

set_canvas size  800 600
set_camera pose  4 3 2 0 0
set_camera pose  60 30 4
set_camera zoom  1.0

set_engine precs 0.0001
set_engine nstep 200
set_engine depth 1 8
set_engine coord -0.2 0.8 0.0 0.0

set_lights add
set_lights position 0 0 0
set_lights ambient  0.1 0.1 0.1
set_lights diffuse  0.1 0.1 0.1
set_lights specular 0.1 0.1 0.1

set_lights add
set_lights position 0 0 0
set_lights ambient  0.1 0.1 0.1
set_lights diffuse  0.1 0.1 0.1
set_lights specular 0.1 0.1 0.1
*/ 
