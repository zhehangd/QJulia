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
  
} *app;





// An engine processes an image (buffer).
class BasicEngine {
public:
  
};

class SurfaceEngine : BasicEngine {
public:
  int     nstep;
  float   precs;
  Vector4 coord;
  
};


bool set_canvas(Console &console,std::vector<std::string> &argv) {
  app->width  = 800;
  app->height = 600;
  return true;
}

bool set_camera(Console &console,std::vector<std::string> &argv) {
  return true;
}

bool set_engine(Console &console,std::vector<std::string> &argv) {
  return true;
}

bool set_system(Console &console,std::vector<std::string> &argv) {
  app->nthreads = 4;
  return true;
}

bool set_lights(Console &console,std::vector<std::string> &argv) {
  return true;
}


int main(int argc,const char **argv) {
  
  
  app = new App();
  
  Console console;
  console.ignore_unknown = true;
  
  console.addCommand("set_canvas",set_canvas);
  console.addCommand("set_camera",set_camera);
  console.addCommand("set_engine",set_engine);
  console.addCommand("set_system",set_system);
  console.addCommand("set_lights",set_lights);

  
  
  if(argc==1)
  {
    std::cout<<"Please pass script files as arguments."<<std::endl;
    return -1;
  }
  
  for(int i=1;i<argc;i++)
  {
    if(std::string(argv[i])=="-e" && (i!=(argc-1)))
      console.eval(argv[i+1]);
    else
      console.runfile(argv[i]);
  }
  
  return 0;
  
}