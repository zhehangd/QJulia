// File:   Camera.h
// Author: Zhehang Ding
// Email:  dingzhehang1993@gmail.com
// Data:   Oct. 15, 2016

#ifndef CAMERA_H_
#define CAMERA_H_

#include <cmath>


// w: world
// c: camera
// p: perspective
// s: screen

class Camera {
    public:
        
    Camera(void);
    
    // Setup extrinsic parameters by "LookAt" format.
    void setupExt(const float *src,const float *dst,const float *up);
    
    // Setup extrinsic parameters by polar coordinates format.
    void setupExt(float h,float v,float r);
    // Setup extrinsic parameters by position and direction
    void setupExt(const float *s, float ha,float va);
    
    // Setup intrinsic parameters.
    void setupInt(float focus,float zmax);
    
    // Setup screen parameters.
    // align_h: if true, align the view by height, otherwise width.
    void setupScn(float w,float h,bool align_h=false);
    
    void projectXsw(float *dst,float *src)const;
    void projectInvXsw(float *dst,float *src)const;
    
    void projectXsc(float *dst,float *src)const;
    void projectInvXsc(float *dst,float *src)const;

    void projectXcw(float *dst,float *src)const;
    void projectXpc(float *dst,float *src)const;
    void projectXsp(float *dst,float *src)const;
    
    void projectInvXcw(float *dst,float *src)const;
    void projectInvXpc(float *dst,float *src)const;
    void projectInvXsp(float *dst,float *src)const;
    
    float getZmax(void)const{return zm;}
    float getFocus(void)const{return f;}
    
    void getPosition(float *dst)const{for(int i=0;i<3;i++)dst[i]=iXcw[i][3];}

    // Intrinsic Parameters
    float f;
    float fz; // -(zmax+f)/zmax.
    float zm; // zmax
    
    // Screen Parameters
    float hw;
    float hh;
    float fs;
    
    //
    float Xcw[4][4];
    float iXcw[4][4];
};


class CameraOrtho : public Camera {
  // prospective feature
};

class CameraPerep : public Camera {
  // prospective feature
};

class CameraStereo : public Camera {
  // prospective feature
};

#endif
