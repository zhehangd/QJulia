// File:   Math.cpp
// Author: Zhehang Ding
// Email:  dingzhehang1993@gmail.com
// Data:   Sep. 07, 2016

#include <cmath>
#include "Math.hpp"

float Vector3::normalize(void)
{
  float m=norm();
  if(m>1e-6)
    (*this)/=m;
  return m;
  
}

Vector3 cross(const Vector3 &v1,const Vector3 &v2)
{
    return Vector3(v1[1]*v2[2]-v1[2]*v2[1],
                   v1[2]*v2[0]-v1[0]*v2[2],
                   v1[0]*v2[1]-v1[1]*v2[0]);
}

Vector3 normalize(const Vector3& v)
{
    float len = v.norm();
    if(len<1e-5)
      return Vector3(1,0,0);
    else
      return v/len;
}

// Dot multiplication.
float dot(const Vector3 &v1,const Vector3 &v2)
{
  return v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2];
}

Matrix4x4  Matrix4x4::operator*(const Matrix4x4& src)const
{
  Matrix4x4 dst;
  for(int i=0;i<4;i++)
    for(int j=0;j<4;j++)
      dst(i,j) = (*this)(i,0)*src(0,j)
                +(*this)(i,1)*src(1,j)
                +(*this)(i,2)*src(2,j)
                +(*this)(i,3)*src(3,j);
  return dst;
}

Vector4  Matrix4x4::operator*(const Vector4& src)const
{
  Vector4 dst;
  for(int i=0;i<4;i++)
    dst(i) = (*this)(i,0)*src(0)
            +(*this)(i,1)*src(1)
            +(*this)(i,2)*src(2)
            +(*this)(i,3)*src(3);
  return dst;
}

Quaternion& Quaternion::operator+=(const Quaternion &src)
{
    a += src.a;
    i += src.i;
    j += src.j;
    k += src.k;
    return *this;
}

Quaternion& Quaternion::operator*=(const Quaternion &src)
{
    float ta = a*src.a - i*src.i - j*src.j - k*src.k;
    float ti = a*src.i + i*src.a + j*src.k - k*src.j;
    float tj = a*src.j - j*src.k + j*src.a + k*src.j;
    float tk = a*src.k + i*src.j - j*src.i + k*src.a;
    a = ta; i = ti; j = tj; k=tk;
    return *this;
}


Quaternion Quaternion::operator+(const Quaternion &src)const
{
    return (Quaternion(*this)+=src);
}

Quaternion Quaternion::operator*(const Quaternion &src)const
{
    return (Quaternion(*this)*=src);
}


std::ostream& operator<<(std::ostream& ss,const Vector3 &v)
{
  auto prec = ss.precision(4);
  ss<<"("<<v[0]<<","<<v[1]<<","<<v[2]<<")";
  ss.precision(prec);
  return ss;
}

