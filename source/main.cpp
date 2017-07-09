#include <iostream>
#include <iomanip>
#include <vector>
#include <memory>

#include "Console.hpp"
#include "Utility.hpp"
#include "Image.hpp"
#include "Perlin3d.hpp"
#include "Camera.hpp"
#include "Math.hpp"




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



int main(int argc,const char **argv) {
  
  
  app = new App();
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
