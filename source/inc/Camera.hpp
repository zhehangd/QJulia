// File:   Camera.h
// Author: Zhehang Ding
// Email:  dingzhehang1993@gmail.com
// Data:   Oct. 15, 2016

#ifndef CAMERA_H_
#define CAMERA_H_

// Coordinate Systems:
// w: world      :  object's native (right-handed) coordinates .
// c: camera     :  coordinate system rotated from the world system.
//                  the origin locates at the camera center.
//                  right-handed coordinate system.
//                  -z axis pointing to the orientation of the camera.
//                   x axis pointing to the right-hand side.
//                   y axis pointing upward.
// p: projection :  left-handed coordinate system projected from the
//                  camera system. The origin and the directions of
//                  the x,y axes keep the same.
// s: screen        x,y are mapped to the screen coordinates.
//                  one of them, y by default, is used as a reference.
//                  y: [1,-1] -> [0,height] (reversed)
//                  x: [-m,m] -> [0,width]
//                  z: identity
//                     m is selected to keep the aspect ratio.
// Methods:
// projectXab:     projecting from system b to system a.
// projectInvXsw:  projecting from a to b.
// setupExt:       a set of methods that set camera's position and orentation.
// setupInt:       a set of methods that set camera's projection algorithm.
// setupScn:       a set of methods that set the size of the screen .

class Camera {
  
public:
        
    Camera(void);
    
    // Set up the extrinsic parameters with "XY Canvas" style.
    void setupExt(void);
    
    // Set up the extrinsic parameters with "LookAt" style.
    void setupExt(const float *src,const float *dst,const float *up);
    
    // Set up the extrinsic parameters with "LookAtOrigin" style.
    void setupExt(float h,float v,float r);
    
    // Set up extrinsic parameters with "GunTurret" style.
    void setupExt(const float *s, float ha,float va);

    // Setup intrinsic parameters.
    virtual void setupInt(float focus,float zmax) {}
    
    // Setup screen parameters.
    // align_h: if true, align the view by height, otherwise width.
    void setupScn(float w,float h,bool align_h=true);
    
    void projectXsw(float *dst,float *src)const;
    void projectInvXsw(float *dst,float *src)const;

    void projectXsc(float *dst,float *src)const;
    void projectInvXsc(float *dst,float *src)const;

    void projectXcw(float *dst,float *src)const;
    void projectXsp(float *dst,float *src)const;

    void projectInvXcw(float *dst,float *src)const;
    void projectInvXsp(float *dst,float *src)const;

    virtual void projectXpc(float *dst,float *src)const
                           {for(int i=0;i<3;i++)dst[i]=src[i];}
    virtual void projectInvXpc(float *dst,float *src)const
                           {for(int i=0;i<3;i++)dst[i]=src[i];}
    
protected:
  
  // Screen Parameters
  float hw;
  float hh;
  float fs;
  
  float Xcw[4][4];
  float iXcw[4][4];
  
};


//  ----------------------------------
//  right-hand       left-hand
//
//      /|              |\
//     / |              |
//    /| | x   ----> x' |
//   /_|_|              |___\
//    f -z              0 z  zmax
//  ===================================
class CameraPersp : public Camera {
  
public:
    CameraPersp(void);
    void setupInt(float focus,float zmax);
    void projectXpc(float *dst,float *src)const;
    void projectInvXpc(float *dst,float *src)const;

private:
    float f;
    float fz; // -(zmax+f)/zmax.
    float zm; // zmax
};


class CameraOrtho : public Camera {
  
public:
    CameraOrtho(void);
    void setupInt(float focus,float zmax);
    void projectXpc(float *dst,float *src)const;
    void projectInvXpc(float *dst,float *src)const;

private:
    float f;
    float zm; // zmax
};


class CameraStereo : public Camera {

};

#endif
