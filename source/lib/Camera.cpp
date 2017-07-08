// File:   Camera.cpp
// Author: Zhehang Ding
// Email:  dingzhehang1993@gmail.com
// Data:   Oct. 15, 2016

#include <cmath>
#include <cstdio>
#include "Camera.hpp"

using namespace std;

float VecDot(const float *v1,const float *v2)
{
  return v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2];
}

void VecNormalize(float *d,const float *s)
{
  float norm = std::sqrt(VecDot(s,s));
  for(int i=0;i<3;i++)
    d[i] = s[i] / norm;
}

void VecMultMatrix(float *dst,const float m[4][4],const float *src)
{
    for(int i=0;i<3;i++)
      dst[i] = src[0]*m[i][0] + src[1]*m[i][1] + src[2]*m[i][2] + m[i][3];
    float w  = src[0]*m[3][0] + src[1]*m[3][1] + src[2]*m[3][2] + m[3][3];
    for(int i=0;i<3;i++)
      dst[i] /= w;
}


Camera::Camera(void)
{
  f  = 1; fz = 1;
  hw = 0; hh = 0; fs = 0;
  Xcw[0][0] = Xcw[1][1] = Xcw[2][2] = Xcw[3][3] = 1;
  Xcw[0][1] = Xcw[0][2] = Xcw[0][3] = 0;
  Xcw[1][0] = Xcw[1][2] = Xcw[1][3] = 0;
  Xcw[2][0] = Xcw[2][1] = Xcw[2][3] = 0;
  Xcw[3][0] = Xcw[3][1] = Xcw[3][2] = 0;
  iXcw[0][0] = iXcw[1][1] = iXcw[2][2] = iXcw[3][3] = 1;
  iXcw[0][1] = iXcw[0][2] = iXcw[0][3] = 0;
  iXcw[1][0] = iXcw[1][2] = iXcw[1][3] = 0;
  iXcw[2][0] = iXcw[2][1] = iXcw[2][3] = 0;
  iXcw[3][0] = iXcw[3][1] = iXcw[3][2] = 0;
}


void Camera::setupExt(const float *src,const float *dst,const float *up)
{
  float camz[3];
  for(int i=0;i<3;i++)
    camz[i] = src[i] - dst[i];
  VecNormalize(camz,camz);
  float camy[3];
  VecNormalize(camy,up);
  float proj = VecDot(camz,camy);
  for(int i=0;i<3;i++)
    camy[i] = camy[i] - proj*camz[i];
  VecNormalize(camy,camy);
  float camx[3];
  camx[0] = (camy[1] * camz[2] - camy[2] * camz[1]);
  camx[1] = (camy[2] * camz[0] - camy[0] * camz[2]);
  camx[2] = (camy[0] * camz[1] - camy[1] * camz[0]);
  
  // Set Xsp.
  Xcw[0][3] = - VecDot(src,camx);
  Xcw[1][3] = - VecDot(src,camy);
  Xcw[2][3] = - VecDot(src,camz);
  Xcw[3][0]=Xcw[3][1]=Xcw[3][2]=0;
  Xcw[3][3]=1;
  for(int i=0;i<3;i++){
    Xcw[0][i] = camx[i];
    Xcw[1][i] = camy[i];
    Xcw[2][i] = camz[i];
  }
  // Set inv(Xsp)
  iXcw[3][0]=iXcw[3][1]=iXcw[3][2]=0;
  iXcw[3][3]=1;
  for(int i=0;i<3;i++){
    iXcw[i][0] = camx[i];
    iXcw[i][1] = camy[i];
    iXcw[i][2] = camz[i];
    iXcw[i][3] = src[i];
  }
}

void Camera::setupExt(float h,float v,float r)
{
  const float deg2rad = 3.1415927f/180.0f;
  float rotHr = h * deg2rad;
  float rotVr = v * deg2rad;
  float rotHc = std::cos(rotHr);
  float rotHs = std::sin(rotHr);
  float rotVc = std::cos(rotVr);
  float rotVs = std::sin(rotVr);
  float src[3] = {r*rotVc*rotHc,r*rotVs,-r*rotVc*rotHs};
  float dst[3] = {0,0,0};
  float up[3]  = {0,1,0};
  setupExt(src,dst,up);
}

// Setup extrinsic parameters by position and direction
void Camera::setupExt(const float *src,float h,float v)
{
  const float deg2rad = 3.1415927f/180.0f;
  float rotHr = h * deg2rad;
  float rotVr = v * deg2rad;
  float rotHc = std::cos(rotHr);
  float rotHs = std::sin(rotHr);
  float rotVc = std::cos(rotVr);
  float rotVs = std::sin(rotVr);
  float dst[3] = {};
  dst[0] = src[0] +  rotVc*rotHc;
  dst[1] = src[1] +  rotVs;
  dst[2] = src[2] + -rotVc*rotHs;
  float up[3]  = {0,1,0};
  setupExt(src,dst,up);
}

void Camera::setupInt(float focus,float zmax)
{
  zm = zmax;
  f  = focus;
  fz = (zmax+focus)/(zmax + 1e-6);
}

void Camera::setupScn(float w,float h,bool align_h)
{
  hw = w / 2;
  hh = h / 2;
  fs = align_h ? hh : hw;
}


void Camera::projectXsw(float *dst,float *src)const
{
  projectXcw(dst,src);
  projectXpc(dst,dst);
  projectXsp(dst,dst);
}

void Camera::projectInvXsw(float *dst,float *src)const
{
  projectInvXsp(dst,src);
  projectInvXpc(dst,dst);
  projectInvXcw(dst,dst);
}

void Camera::projectXsc(float *dst,float *src)const
{
  projectXpc(dst,dst);
  projectXsp(dst,dst);
}

void Camera::projectInvXsc(float *dst,float *src)const
{
  projectInvXsp(dst,src);
  projectInvXpc(dst,dst);
}

void Camera::projectXcw(float *dst,float *src)const
{
  float s4[] = {src[0],src[1],src[2],1};
  float d4[] = {}; VecMultMatrix(d4,Xcw,s4);
  for(int i=0;i<3;i++)
    dst[i] = d4[i];
}

void Camera::projectXpc(float *dst,float *src)const
{
  float w = f - src[2];
  dst[0]  = f  * src[0] / w;
  dst[1]  = f  * src[1] / w;
  dst[2]  =-fz * src[2] / w;
}

void Camera::projectXsp(float *dst,float *src)const
{
  dst[0] =   src[0] * fs + hw;
  dst[1] = - src[1] * fs + hh;
}

void Camera::projectInvXcw(float *dst,float *src)const
{
  float s4[] = {src[0],src[1],src[2],1};
  float d4[] = {}; VecMultMatrix(d4,iXcw,s4);
  for(int i=0;i<3;i++)
    dst[i] = d4[i];
}

void Camera::projectInvXpc(float *dst,float *src)const
{
  dst[2] = src[2] * f / (-fz + src[2]);
  float w = f - dst[2];
  dst[0] = src[0] * w / f;
  dst[1] = src[1] * w / f;
}

void Camera::projectInvXsp(float *dst,float *src)const
{
  dst[0] =   (src[0] - hw) / fs;
  dst[1] = - (src[1] - hh) / fs;
}