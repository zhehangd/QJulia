// File:   TriInterp.cpp
// Author: Zhehang Ding
// Email:  dingzhehang1993@gmail.com
// Data:   Oct. 15, 2016

#include "Primitive.hpp"
#include <cmath>
#include <utility> // swap

using namespace std;

// Sort the vertices
void vertexSort(const float *v1, const float *v2, const float *v3, int &o1,int &o2,int &o3)
{
	o1 = 0; o2 = 1; o3 = 2;
	// This code sorts the three vertices of a tri following these principles:
	// 1. The vertices are in clockwise order.
	// 2. The first vertex is the top one of the triangle (minimal y).
	// 3. For a triangle with a flat top, the top-left vertex is at first.
	// Bubble Sorting
	// In ascending order with respect to y coordinates.
	if (v1[1] > v2[1]){ swap(o1,o2); swap(v1,v2); }
	if (v2[1] > v3[1]){ swap(o2,o3); swap(v2,v3); }
	if (v1[1] > v2[1]){ swap(o1,o2); swap(v1,v2); }

	// Now v[a][1] <= v[b][1] <= v[c][1]
	// Do another test to handle the flat-top case.
	if (v1[1] == v2[1] && v1[0]>v2[0]){ swap(o1,o2); swap(v1,v2); }
	// Now we have satisfied principle 2 and 3.
	// Pick the mid-y vertex and find the intersection on the oppsite side.
	float dx = v3[0] - v1[0];
	float dy = v3[1] - v1[1];
	float cmpx2 = v2[0] * dy;                       // The test value of v2.
	float cmpxi = dx*v2[1] + dy*v1[0] - dx*v1[1];   // The test value of the intersection.
	                                                // Now being clockwise is equivalent to (cmpx2 > cmpxi).
	if (cmpx2 < cmpxi){ swap(o2,o3); swap(v2,v3); } // So we make a possible swap of the last two vertices to achieve the principle 1.
}

void TriInterp::setup(const float *v1,const float *v2,const float *v3,
                      float *a1,float *a2,float *a3,int n)
{
  setup(v1[0],v1[1],v2[0],v2[1],v3[0],v3[1],a1,a2,a3,n);
}

void TriInterp::setup(float x1,float y1,float x2,float y2,float x3,float y3,
                      float *a1,float *a2,float *a3,int n)
{
 // First, sort the vertices.
	int     o1,o2,o3;
	float*  plist[3]    = { a1,a2,a3 };
	float  xylist[3][2] = {{x1,y1},{x2,y2},{x3,y3}};
	
	vertexSort(xylist[0],xylist[1],xylist[2],o1,o2,o3);

	x1 = xylist[o1][0];  y1 = xylist[o1][1];  a1 = plist[o1];
	x2 = xylist[o2][0];  y2 = xylist[o2][1];  a2 = plist[o2];
	x3 = xylist[o3][0];  y3 = xylist[o3][1];  a3 = plist[o3];

	// Compute the bounding box.
	bbox[0] = std::min(std::min(x1, x2), x3);
	bbox[1] = y1;
	bbox[2] = std::max(std::max(x1, x2), x3);
	bbox[3] = std::max(y2, y3);

	// We need a criterion to render the pixels exactly on the edge.
	// The goal is to make sure each pixel belongs to only one triangle.
	// So we only draw on-edge pixels on the left side edges of the triangle,
	// and the ones on the bottom flat edges. We omit the ones on the right
	// side edges and the top flat edges. That is, we classify the bottom flat
	// edges as left and classify the top flat edges as right.
	// If the previous sorting principles are satisfied, L12 is always right,
	// and L31 is always left. We only need to test L23.
	//isL23Right = (y2 < y3);
	isL23Right = (y2 < y3);

	// Assume the well sorted vertices.
	// Compute the inside/outside test equation.
	//float k[3][3] = {};
	Me[0][0] = y2 - y1;
	Me[0][1] = x1 - x2;
	Me[0][2] = (x2 - x1)*y1 - (y2 - y1)*x1;
	Me[1][0] = y3 - y2;
	Me[1][1] = x2 - x3;
	Me[1][2] = (x3 - x2)*y2 - (y3 - y2)*x2;
	Me[2][0] = y1 - y3;
	Me[2][1] = x3 - x1;
	Me[2][2] = (x1 - x3)*y3 - (y1 - y3)*x3;

	// Compute the interpolation equation.
	Ik.resize(n);
	for(int i=0;i<n;i++)
	{
		float dx12 = x2 - x1, dy12 = y2 - y1, dz12 = a2[i] - a1[i];
		float dx13 = x3 - x1, dy13 = y3 - y1, dz13 = a3[i] - a1[i];
		float A = (dy12*dz13 - dz12*dy13);
		float B = (dz12*dx13 - dx12*dz13);	
		float C = (dx12*dy13 - dy12*dx13);
		float D = -(A*x1 + B*y1 + C*a1[i]);
		Ik[i][0] = A;
		Ik[i][1] = B;
		Ik[i][2] = C;
		Ik[i][3] = D;
	}
}

void TriInterp::interpolate(float x,float y,float *v)
{
	for(int i=0;i<Ik.size();i++)
		v[i] = -(Ik[i][0]*x+Ik[i][1]*y+Ik[i][3])/Ik[i][2];
}

bool TriInterp::evaluate(float x,float y)
{
	float v1 = (Me[0][0] * x + Me[0][1] * y + Me[0][2]);
	float v2 = (Me[1][0] * x + Me[1][1] * y + Me[1][2]);
	float v3 = (Me[2][0] * x + Me[2][1] * y + Me[2][2]);
	return ((v1<0) && (v3 <= 0) && (isL23Right ? (v2<0) : (v2 <= 0)));
}


Bresenham::Bresenham(void)
{
  setup(0,0,0,0);
}

//#include <iostream>


void Bresenham::setup(int x1,int y1,int x2,int y2,float *a1,float *a2,int n)
{
  // Flip x-y if the slope > 1.
  fx = &tx;
  fy = &ty;
  bool flag1= std::abs(y2-y1)>std::abs(x2-x1);
  if(flag1==true)
  {
    std::swap(x1,y1);
    std::swap(x2,y2);
    fx = &ty;
    fy = &tx;
  }
  // Flip 1 and 2 if x1>x2;
  bool flag2 = x1>x2;
  if(flag2==true)
  {
    std::swap(x1,x2);
    std::swap(y1,y2);
    std::swap(a1,a2);
  }
  //
  dx    = x2-x1;
  dy    = std::abs(y2 - y1);
  thres = (x2-x1) / 2;
  step  = (y2>y1)?1:-1;
  // Setup tx and ty
  tx = flag1 ? y1 : x1;
  ty = flag1 ? x1 : y1;
  srt = x1;
  end = x2;
  
  Iv.resize(n);
  for(int i=0;i<n;i++){
    Iv[i][0] = (a2[i]-a1[i])/(end-srt);
    Iv[i][1] = a1[i] - srt*Iv[i][0];
  }
}

bool Bresenham::next(int *x,int *y,float *a)
{
  if ((*fx)>end)
    return false;
  *x = tx;
  *y = ty;
  if (a!=0)
    for(int i=0;i<Iv.size();i++)
      a[i] = Iv[i][0]*(*fx) + Iv[i][1];
  thres -= dy;
  if(thres<0)
  {
      thres += dx;
      (*fy) += step;
  }
  (*fx)++;
  return true;
}