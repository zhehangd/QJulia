// File:   Perlin3d.cpp
// Author: Zhehang Ding
// Email:  dingzhehang1993@gmail.com
// Data:   Oct. 02, 2016

#include <cstdint>
#include <cmath>

#include "Perlin3d.hpp"

using namespace std;

float Perlin3D::interpolate(float a,float b,float x)const
{
  x = (3*x*x-2*x*x*x);
  return a*(1-x) + b*x;
}

void Perlin3D::setseed(int x)
{
  uint32_t n = 2120648927 ^ (x<<13) ^ (x);
  n = (n * (n * n * 2088609121 + 3172726747) + 2547849113);
  n = (n * (n * n * 2547849113 + 1012064509) + 5120638361);
  seed = n;
}

void Perlin3D::getGradient(int xyz[3],float g[3])const
{
  uint32_t nx = (xyz[0]<<13) ^ xyz[0];
  uint32_t ny = (xyz[1]<<13) ^ xyz[1];
  uint32_t nz = (xyz[2]<<13) ^ xyz[2];
  nx = (nx * (nx * nx * 15731 + 789221) + 1376312589);
  ny = (ny * (ny * ny * 75931 + 366161) + 3641089513);
  nz = (nz * (nz * nz * 34631 + 137077) + 3860646553);
  uint32_t na = (((7963022183 * nx + 1888775729)
                              * ny + 1798155071)
                              * nz + 2547849113)
                              * seed + 1514959861;
  uint32_t nb = 3862515851 * na + 278717; 
                  
  float va = ((float)(na & 0xffff) / 0xffff) * 3.14159 * 2;
  float vb = ((float)(nb & 0xffff) / 0xffff) * 3.14159 * 2;

  g[0] = cos(va)*cos(vb);
  g[1] = cos(va)*sin(vb);
  g[2] = sin(va);
}

float Perlin3D::perlin(float x,float y,float z,float stride)const
{
  float sx = (float) x / stride;
  float sy = (float) y / stride;
  float sz = (float) z / stride;
  int x0 = (int)floor(sx);
  int y0 = (int)floor(sy);
  int z0 = (int)floor(sz);
  int x1 = x0 + 1;
  int y1 = y0 + 1;
  int z1 = z0 + 1;
  
  // Cube vertices.
  int vtx[8][3] = 
      {{x0,y0,z0},{x0,y0,z1},{x0,y1,z0},{x0,y1,z1},    
       {x1,y0,z0},{x1,y0,z1},{x1,y1,z0},{x1,y1,z1}};
  
  float val[8];
  for(int i=0;i<8;i++)
  {
    float grd[3]; getGradient(vtx[i],grd);
    val[i] =    (sx-vtx[i][0])*grd[0]
               +(sy-vtx[i][1])*grd[1]
               +(sz-vtx[i][2])*grd[2];
  }
  
  float px0z0 = interpolate(val[0],val[2],sy-y0);
  float px0z1 = interpolate(val[1],val[3],sy-y0);
  float px0   = interpolate( px0z0, px0z1,sz-z0);
  float px1z0 = interpolate(val[4],val[6],sy-y0);
  float px1z1 = interpolate(val[5],val[7],sy-y0);
  float px1   = interpolate( px1z0, px1z1,sz-z0);
  float p     = interpolate(   px0,   px1,sx-x0);
  
  return p;
}