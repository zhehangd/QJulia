// File:   Math.cpp
// Author: Zhehang Ding
// Email:  dingzhehang1993@gmail.com
// Data:   Feb. 07, 2016

#include <cmath>
#include "Math.h"


// Construct zero vector.
Vector3::Vector3(void)
{
    v[0]=v[1]=v[2]=0;
}

// Construct by numbers
Vector3::Vector3(float x,float y,float z)
{
    v[0]=x;v[1]=y;v[2]=z;
}

// Construct by array.
Vector3::Vector3(float *s)
{
    v[0]=s[0];v[1]=s[1];v[2]=s[2];
}

float Vector3::norm1(void)const
{
    return std::abs(v[0])+std::abs(v[1])+std::abs(v[2]);
}

float Vector3::norm2(void)const
{
    return std::sqrt(((*this)*(*this)).sum());
}

float Vector3::sum(void)
{
    return v[0]+v[1]+v[2];
}

// Cross multiplication.
Vector3 cross(const Vector3 &v1,const Vector3 &v2)
{
    return Vector3(v1[1]*v2[2]-v1[2]*v2[1],
                   v1[2]*v2[0]-v1[0]*v2[2],
                   v1[0]*v2[1]-v1[1]*v2[0]);
}

Vector3 normalize(const Vector3& v)
{
    return v/v.norm();
}

float dot(const Vector3 &v1,const Vector3 &v2)
{
    return (v1*v2).sum();
}

Camera::Camera(void):zn(0.0f),zf(5.0f),f(1.0f)
{
    for(int i=0;i<16;i++)
        ((float*)m)[i] = 0;
    for(int i=0;i<4;i++)
        m[i][i] = 1;
    for(int i=0;i<16;i++)
        ((float*)mInv)[i] = 0;
    for(int i=0;i<4;i++)
        mInv[i][i] = 1;
}


void Camera::setupExt(Vector3 s,Vector3 d,Vector3 u)
{
    // We define direction of negative z-axis is what the cameera points to.
    // y-axis is the upper direction.
    // x-y-z is a right-handed system.
    Vector3 x,y,z,c;
    z = normalize(s-d); // the dst point is at the negative direction of z-axis.
    y = u - z*dot(u,z); // othonormalize the upper direction.
    y = normalize(y);
    x = cross(y,z);      // x is orthogonal to both y and z.
    c[0] = - dot(x,s); // Get the position with respect of camera basis.
    c[1] = - dot(y,s); // Since it is the origin(camera) moves
    c[2] = - dot(z,s); // The direction is negative (to objects).
    // Generate the matrix using change of basis theory.
    // [Modelview -> Camera] = inv([C->M]) without translation.
    // Calculate [C->M] is easy to get, just [x,y,z].
    // And it is an isometry, inv=transpose, so
    // [M->C]=[x,y,z]T = [x;y;z].
    // Then extend it to affine form, add translation part.
    m[0][0]=x[0]; m[0][1]=x[1]; m[0][2]=x[2]; m[0][3]=c[0];
    m[1][0]=y[0]; m[1][1]=y[1]; m[1][2]=y[2]; m[1][3]=c[1];
    m[2][0]=z[0]; m[2][1]=z[1]; m[2][2]=z[2]; m[2][3]=c[2];
    m[3][0]=m[3][1]=m[3][2]=0;
    m[3][3]=1;
    
    // Inverse
    mInv[0][0]=x[0]; mInv[0][1]=y[0]; mInv[0][2]=z[0]; mInv[0][3]=s[0];
    mInv[1][0]=x[1]; mInv[1][1]=y[1]; mInv[1][2]=z[1]; mInv[1][3]=s[1];
    mInv[2][0]=x[2]; mInv[2][1]=y[2]; mInv[2][2]=z[2]; mInv[2][3]=s[2];
    mInv[3][0]=mInv[3][1]=mInv[3][2]=0;
    mInv[3][3]=1;
}

void Camera::setupExt(float h,float v,float r)
{
    const float deg2rad = 3.1415927f/180.0f;
    float rotHr = h * deg2rad;
    float rotVr = v * deg2rad;
    float rotHc = cos(rotHr);
    float rotHs = sin(rotHr);
    float rotVc = cos(rotVr);
    float rotVs = sin(rotVr);
    Vector3 pos = Vector3(rotVc*rotHc,rotVs,-rotVc*rotHs)*r;
    setupExt(pos,Vector3(0,0,0),Vector3(0,1,0));
}

// Setup extrinsic parameters by position and direction
void Camera::setupExt(Vector3 s, float h,float v)
{
    const float deg2rad = 3.1415927f/180.0f;
    float rotHr = h * deg2rad;
    float rotVr = v * deg2rad;
    float rotHc = cos(rotHr);
    float rotHs = sin(rotHr);
    float rotVc = cos(rotVr);
    float rotVs = sin(rotVr);
    Vector3 dir = Vector3(rotVc*rotHc,rotVs,-rotVc*rotHs);
    setupExt(s,s+dir,Vector3(0,1,0));
}

void Camera::setupInt(float focus,float zNear,float zFar)
{
    f  = focus;
    zf = zFar;
    zn = zNear;
    
    // Perspective
    /*
    for(int i=0;i<16;i++)
        ((float*)p)[i] = 0.0f;
    p[0][0] = p[1][1] = f;
    p[2][2] = -(fz+nz)/(fz-nz);
    p[2][3] = -2*nz/(fz-nz);
    m[3][3] = -1;
    
    // Inverse.
    for(int i=0;i<16;i++)
        ((float*)pInv)[i] = 0.0f;
    pInv[0][0] = pInv[1][1] = 1.0f/f;
    
    pInv[0][0]=1/f; pInv[0][1]=y[0]; pInv[0][2]=z[0]; pInv[0][3]=s[0];
    pInv[1][0]=0; pInv[1][1]=y[1]; pInv[1][2]=z[1]; pInv[1][3]=s[1];
    pInv[2][0]=x[2]; pInv[2][1]=y[2]; pInv[2][2]=z[2]; pInv[2][3]=s[2];
    pInv[3][0]=pInv[3][1]=pInv[3][2]=0;
    pInv[3][3]=1;*/
}

Vector4 matrixMult4x4(const float m[4][4],const Vector4 &v)
{
    Vector4 out;
    out[0] = v[0]*m[0][0] + v[1]*m[0][1] + v[2]*m[0][2] + v[3]*m[0][3];
    out[1] = v[0]*m[1][0] + v[1]*m[1][1] + v[2]*m[1][2] + v[3]*m[1][3];
    out[2] = v[0]*m[2][0] + v[1]*m[2][1] + v[2]*m[2][2] + v[3]*m[2][3];
    out[3] = v[0]*m[3][0] + v[1]*m[3][1] + v[2]*m[3][2] + v[3]*m[3][3];
    return out;
}

Vector4 Camera::project(Vector4 v)const
{
    return matrixMult4x4(m,v);
}

Vector4 Camera::projectInv(Vector4 v)const
{
    return matrixMult4x4(mInv,v);
}


