// File:   Math.h
// Author: Zhehang Ding
// Email:  dingzhehang1993@gmail.com
// Data:   Feb. 07, 2016

#ifndef MATH_H_
#define MATH_H_

#include <cmath>
   
class Vector3
{
public:
    // Construct zero vector.
    Vector3(void);
    // Construct by numbers
    Vector3(float x,float y,float z=0);
    // Construct by array.
    Vector3(float *s);
    
    // return Euclidean length.
    float norm(void)const{return norm2();}
    float norm1(void)const; // The length
    float norm2(void)const; // The square of the length.
    
    // return the summation of the elements.
    float sum(void);
    
    // Access.
    inline float  operator[](const int i)const{return v[i];}
    inline float& operator[](const int i){return v[i];}
    // Compound assignment operators for vectors.
    inline Vector3& operator+=(const Vector3 &src);
    inline Vector3& operator-=(const Vector3 &src);
    inline Vector3& operator*=(const Vector3 &src);
    inline Vector3& operator/=(const Vector3 &src);
    
    // Negation operator.
    inline Vector3  operator-(void)const;
    
    // Binary arithmetic operators for vectors.
    inline Vector3  operator+(const Vector3 &src)const;
    inline Vector3  operator-(const Vector3 &src)const;
    inline Vector3  operator*(const Vector3 &src)const;
    inline Vector3  operator/(const Vector3 &src)const;
    
    // Compound assignment operators for numbers.
    inline Vector3& operator+=(float a);
    inline Vector3& operator-=(float a);
    inline Vector3& operator*=(float a);
    inline Vector3& operator/=(float a);
    
    
    // Binary arithmetic operators for numbers.
    inline Vector3  operator+(float a)const;
    inline Vector3  operator-(float a)const;
    inline Vector3  operator*(float a)const;
    inline Vector3  operator/(float a)const;
    
    // Data
    float v[3];

};

// Cross multiplication v1xv2.
Vector3 cross(const Vector3 &v1,const Vector3 &v2);

// Normalize the vector.
Vector3 normalize(const Vector3& v);

// Dot multiplication.
float dot(const Vector3 &v1,const Vector3 &v2);




Vector3& Vector3::operator+=(const Vector3 &src)
{
    v[0]+=src[0];v[1]+=src[1];v[2]+=src[2];
    return *this;
}

Vector3& Vector3::operator-=(const Vector3 &src)
{
    v[0]-=src[0];v[1]-=src[1];v[2]-=src[2];
    return *this;
}

Vector3& Vector3::operator*=(const Vector3 &src)
{
    v[0]*=src[0];v[1]*=src[1];v[2]*=src[2];
    return *this;
}

Vector3& Vector3::operator/=(const Vector3 &src)
{
    v[0]/=src[0];v[1]/=src[1];v[2]/=src[2];
    return *this;
}

Vector3  Vector3::operator-(void)const
{
    return Vector3(-v[0],-v[1],-v[2]);
}

Vector3  Vector3::operator+(const Vector3 &src)const
{
    return (Vector3(*this)+=src);
}

Vector3  Vector3::operator-(const Vector3 &src)const
{
    return (Vector3(*this)-=src);
}

Vector3  Vector3::operator*(const Vector3 &src)const
{
    return (Vector3(*this)*=src);
}

Vector3  Vector3::operator/(const Vector3 &src)const
{
    return (Vector3(*this)/=src);
}

Vector3& Vector3::operator+=(float a)
{
    v[0]+=a;v[1]+=a;v[2]+=a;
    return *this;
}

Vector3& Vector3::operator-=(float a)
{
    v[0]-=a;v[1]-=a;v[2]-=a;
    return *this;
}

Vector3& Vector3::operator*=(float a)
{
    v[0]*=a;v[1]*=a;v[2]*=a;
    return *this;
}

Vector3& Vector3::operator/=(float a)
{
    v[0]/=a;v[1]/=a;v[2]/=a;
    return *this;
}

Vector3  Vector3::operator+(float a)const
{
    return (Vector3(*this)+=a);
}


Vector3  Vector3::operator-(float a)const
{
    return (Vector3(*this)-=a);
}

Vector3  Vector3::operator*(float a)const
{
    return (Vector3(*this)*=a);
}

Vector3  Vector3::operator/(float a)const
{
    return (Vector3(*this)/=a);
}


struct Vector4
{
    Vector4(void){v[0]=v[1]=v[2]=v[3]=0;}
    Vector4(const Vector3& v3,float w=1){for(int i=0;i<3;i++)v[i]=v3[i];v[3]=w;}
    Vector4(float x,float y,float z,float w=1){v[0]=x;v[1]=y;v[2]=z;v[3]=w;}
    Vector4(float *p,float w=1){v[0]=p[0];v[1]=p[1];v[2]=v[2];v[3]=w;}
    Vector3 v3(void){return Vector3(v);}
    const Vector4& operator=(const Vector3& v3){(*this)=Vector4(v3);}
    
    float v[4];
    inline float  operator[](const int i)const{return v[i];}
    inline float& operator[](const int i){return v[i];}
};

class Quaternion
{
    public:
        Quaternion(void):a(0),i(0),j(0),k(0){}
        Quaternion(float a,float i,float j,float k):a(a),i(i),j(j),k(k){}
        
        inline Quaternion& operator+=(const Quaternion &src);
        inline Quaternion& operator*=(const Quaternion &src);
        
        inline Quaternion operator+(const Quaternion &src)const;
        inline Quaternion operator*(const Quaternion &src)const;
        
        inline float norm1(){return std::fabs(a)+std::fabs(i)+std::fabs(j)+std::fabs(k);};
        
        float a,i,j,k;
};


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

class Camera
{
    public:
        
    Camera(void);
    
    // Setup extrinsic parameters by "LookAt" format.
    void setupExt(Vector3 s,Vector3 d,Vector3 u=Vector3(0,1,0));
    // Setup extrinsic parameters by polar coordinates format.
    void setupExt(float h,float v,float r);
    // Setup extrinsic parameters by position and direction
    void setupExt(Vector3 s, float h,float v);
    // Setup intrinsic parameters.
    void setupInt(float focus,float zNear,float zFar);
    
    Vector4 project(Vector4 v)const;
    Vector4 projectInv(Vector4 v)const;
    
    // Focus.
    float f;
    // zmin and zmax.
    float zn,zf;
    // Transform matrix (row priority).
    // m:    world  coordinates to camera coordinates.
    // mInv: camera coordinates to world  coordinates.
    // p:    perspective transform. (not implemented yet.)
    // pInv: inverse perspective transform.
    float m[4][4];
    float mInv[4][4];
    float pInv[4][4];
};






#endif
