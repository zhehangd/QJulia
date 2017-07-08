// File:   TriInterp.h
// Author: Zhehang Ding
// Email:  dingzhehang1993@gmail.com
// Data:   Oct. 15, 2016

#ifndef TRIINTERP_H_
#define TRIINTERP_H_

#include <vector>
#include <array>


class TriInterp
{
public:
  // Given three 2-d vertices of a triangle, 
  // prepare for interpolation inside the triangle.
  void setup(const float *v1,const float *v2,const float *v3,
             float *a1,float *a2,float *a3,int n);
  void setup(float x1,float y1,float x2,float y2,float x3,float y3,
             float *a1,float *a2,float *a3,int n);  
  
  void interpolate(float x,float y,float *v);
  
  bool evaluate(float x,float y);
  
  float xmin(void){return bbox[0];}
  float ymin(void){return bbox[1];}
  float xmax(void){return bbox[2];}
  float ymax(void){return bbox[3];}
  
  // Bounding box of the triangle.
  float bbox[4];
  // A 3x3 Matrix used to test a pixel being inside/outside.
  float Me[3][3];
  // Interpolation
  std::vector<std::array<float,4>> Ik;
  // Triangle type.
  bool isL23Right;
};


class Bresenham
{
public:
  Bresenham(void);
  void rewind(void){(*fx)=srt;}
  void setup(int x1,int y1,int x2,int y2,float *a1=0,float *a2=0,int n=0);
  bool next(int *x,int *y,float *a=0);
  
  int tx,ty;
  int dx,dy;
  int thres;
  int step;
  int srt,end;
  int *fx,*fy;
  std::vector<std::array<float,2>> Iv;
};


#endif
