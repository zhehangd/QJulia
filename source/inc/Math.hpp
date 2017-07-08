// File:   Math.h
// Author: Zhehang Ding
// Email:  zhehangd@gmail.com
// Data:   Feb. 07, 2016

#ifndef MATH_H_
#define MATH_H_

#include <cmath>
#include <iostream>

class Vector3
{
public:
    // Construct zero vector.
    Vector3(void){zeros();}
    // Construct by numbers
    Vector3(float x,float y,float z=0){v[0]=x;v[1]=y;v[2]=z;}
    // Construct by array.
    Vector3(float *s){v[0]=s[0];v[1]=s[1];v[2]=s[2];}
    
    // Access.
    float  operator[](const int i)const{return v[i];}
    float& operator[](const int i){return v[i];}
    float  operator()(const int i)const{return v[i];}
    float& operator()(const int i){return v[i];}
    float* ptr(){return v;}
    
    
    float normalize(void);
    float norm(void)const{return std::sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);}
    void  zeros(void){v[0]=v[1]=v[2]=0;}
    void  ones(void) {v[0]=v[1]=v[2]=1;}
    
    // Negation operator.
    Vector3  operator-(void)const{return Vector3(-v[0],-v[1],-v[2]);}
    
    #ifndef TEMPLATE_V3_OPERATOR
    #define TEMPLATE_V3_OPERATOR(TYPE1,TYPE2)                   \
    Vector3& operator TYPE1 (const Vector3 &src){               \
        v[0] TYPE1 src[0];                                      \
        v[1] TYPE1 src[1];                                      \
        v[2] TYPE1 src[2];                                      \
        return *this;                                           \
    }                                                           \
    Vector3 operator TYPE2 (const Vector3 &src)const{           \
        return (Vector3(*this) TYPE1 src);                      \
    }                                                           \
    Vector3& operator TYPE1 (float a){                          \
        v[0] TYPE1 a;                                           \
        v[1] TYPE1 a;                                           \
        v[2] TYPE1 a;                                           \
        return *this;                                           \
    }                                                           \
    Vector3  operator TYPE2 (float a)const{                     \
        return (Vector3(*this) TYPE1 a);                        \
    }
    #endif

    TEMPLATE_V3_OPERATOR(+=,+)
    TEMPLATE_V3_OPERATOR(-=,-)
    TEMPLATE_V3_OPERATOR(*=,*)
    TEMPLATE_V3_OPERATOR(/=,/)
    
    // Data
    float v[3];

};

// Cross multiplication v1xv2.
Vector3 cross(const Vector3 &v1,const Vector3 &v2);

// Normalize the vector.
Vector3 normalize(const Vector3& v);

// Dot multiplication.
float dot(const Vector3 &v1,const Vector3 &v2);

std::ostream& operator<<(std::ostream& ss,const Vector3 &v);

struct Vector4
{
    Vector4(void){v[0]=v[1]=v[2]=v[3]=0;}
    Vector4(const Vector3& v3,float w=1){for(int i=0;i<3;i++)v[i]=v3[i];v[3]=w;}
    Vector4(float x,float y,float z,float w=1){v[0]=x;v[1]=y;v[2]=z;v[3]=w;}
    Vector4(float *p,float w=1){v[0]=p[0];v[1]=p[1];v[2]=p[2];v[3]=w;}
    Vector3 v3(void){return Vector3(v);}
    const Vector4& operator=(const Vector3& v3){(*this)=Vector4(v3);}
    
    float v[4];
    inline float  operator[](const int i)const{return v[i];}
    inline float& operator[](const int i){return v[i];}
    inline float  operator()(const int i)const{return v[i];}
    inline float& operator()(const int i){return v[i];}
};

class Matrix4x4
{
  public:
    Matrix4x4(void){for(int i=0;i<16;i++)m[i]=0;}
    Matrix4x4(float *src){for(int i=0;i<16;i++)m[i]=src[i];}
    
    float  operator[](unsigned int i)const{return m[i];}
    float& operator[](unsigned int i){return m[i];}
    float  operator()(unsigned int i)const{return m[i];}
    float& operator()(unsigned int i){return m[i];}
    float  operator()(unsigned int r,unsigned int c)const{return m[r*4+c];}
    float& operator()(unsigned int r,unsigned int c){return m[r*4+c];}
    
    void identity(void){for(int i=0;i<16;i++)m[i]=0;m[0]=m[4]=m[8]=m[12]=1;}
    
    Matrix4x4& operator*=(const Matrix4x4& src){*this=*this*src;}
    Matrix4x4  operator*(const Matrix4x4& src)const;
    Vector4    operator*(const Vector4& src)const;

    
    float m[16];
};


class Quaternion
{
  public:
    Quaternion(void):a(0),i(0),j(0),k(0){}
    Quaternion(float a,float i,float j,float k):a(a),i(i),j(j),k(k){}
    
    Quaternion& operator+=(const Quaternion &src);
    Quaternion& operator*=(const Quaternion &src);
    Quaternion& operator*=(float v){a*=v;i*=v;j*=v;k*=v;}
    
    Quaternion operator+(const Quaternion &src)const;
    Quaternion operator*(const Quaternion &src)const;
    
    float norm1(){return std::fabs(a)+std::fabs(i)+std::fabs(j)+std::fabs(k);};
    
    float a,i,j,k;
};




#endif
