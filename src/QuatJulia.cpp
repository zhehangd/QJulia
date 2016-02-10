// File:   QuatJulia.cpp
// Author: Zhehang Ding
// Email:  dingzhehang1993@gmail.com
// Data:   Feb. 09, 2016

#include <cfloat>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include "Image.h"
#include "Math.h"
#include "QuatJulia.h"

using namespace std;

// Iteration of q=q^2+c formula
// >0 converge
// =0 unconverge
// <0 rejected
int quatIteration(Quaternion q,Quaternion c,int thres)
{
    // If the coordinates exceed a threshold.
    // We directly return false to save time.
    if(q.norm1()>2)
        return -1;
    for(int i=0;i<thres;i++)
    {
        q = q*q + c;
        if(q.norm1()>10.0f)
            return 0;
    }
    return 1;
}

// division:    How many steps it takes to go through the searching distance.
// precision:   The localization accuracy after finding a rough estimation.
// threshold:   How many iteration to take to decide converence.
Vector3 quatSearch(float x,float y,const Camera &cam,Quaternion c,int div,float prec,int thres)
{
    int i;
    // Search the region.
    Vector3 v=cam.projectInv(Vector3(x,y,-cam.zn)).v3();
    Vector3 p=v;
    for(i=0;i<div;i++)
    {
        float   d = cam.zn+(cam.zf-cam.zn)*(float)i/(div-1);
        float cz = (-d);
        float cx = -x*cz;
        float cy = -y*cz;
        // cx=x;cy=y;cz=-d; // <- Use this to switch back to Orthogonal Proj.
        v = Vector3(cx,cy,cz);
        v = cam.projectInv(v).v3();
        // Construct the quaternion and find if inside the fractal.
        if(quatIteration(Quaternion(v[0],v[1],v[2],0),c,thres)>0)
            break;
        p = v;
    }
    
    if(i==div) // Comes to end.
        return Vector3(INFINITY,INFINITY,INFINITY);
    // Refine the result. (localization)
    Vector3 ub = v;
    Vector3 lb = p;
    while((ub-lb).norm1()>prec)
    {
        //std::cout<<(ub-lb).norm1()<<" "<<std::flush;
        Vector3 v = (ub+lb)/2;
        if(quatIteration(Quaternion(v[0],v[1],v[2],0),c,thres)>0)
            ub = v;
        else
            lb = v;
    }
    return (ub+lb)/2;
}

Image qSurfaceGenerator(const qSurfaceGeneratorParm &parm,const Camera &cam)
{
    int div  = parm.div;
    int thres = parm.thres;
    float prec  = parm.preci;
    Quaternion qc = parm.qc;

    // Pre-compute parameters.
    const int   w  = parm.width,  hw = w/2;
    const int   h  = parm.height, hh = h/2;
    const float rf = 1.0f / cam.f;
    Image  image(w,h,3,IMAGE_FLOAT); // The background has inifity value.
    // Progress indicator.
    int progress = 10;
    // Scan the image
    for(ROIScanner scan(image);scan.isReady();scan.next())
    {
        // Convert pixel location to camera coordinate.
        int r = scan.currentRow();
        int c = scan.currentCol();
        float x = rf*( c-hw)/hh; // to keep ratio constant, both divided by h/2.
        float y = rf*(-r+hh)/hh;
        // Run search algorithm to find the first surface point.
        Vector3 v  = quatSearch(x,y,cam,qc,div,prec,thres).v;
        // Write to the buffer.
        scan.setPixel(v.v);
        
        // Show the progress.
        if((100*r/h)>=progress)
        {
            cout<<progress<<"%..."<<flush;
            progress += 10;
        }
    }
    cout<<endl;
    return image;
}

// This function compute normal vectors from the surface map.
Image qComputeNormal(Image surf)
{
    // New image.
    Image norm; norm.emptyFrom(surf);
    // Compute the normal vecotrs.
    // We do this by taking discrete difference of ajacant pixels.
    // So high rendering resolution is necessary for good quality.
    
    // Scan the images(src,dst) except for the last row and column.
    ROIScanner ss(surf,0,0,surf.width-1,surf.height-1);
    ROIScanner sd(norm,0,0,norm.width-1,norm.height-1);
    for(;sd.isReady();ss.next(),sd.next())
    {
        // If the pixel is Inf, indicating there is no convergent point.
        // so we assign the zero vector.
        if(isinf(ss.value<float>()))
        {
            sd.setPixel(Vector3().v);
            continue;
        }
        
        // Get the pixel on the right and the one below the current pixel.
        float* nb[2];
        nb[0] = ss.ptr<float>()+surf.dx;
        nb[1] = ss.ptr<float>()+surf.dy;
        // If the any neightbor is inf, assign zero vector.
        // Only the first coord is test.
        if(isinf(nb[0][0])||isinf(nb[1][0]))
        {
            Vector3 zero;
            sd.setPixel(Vector3().v);
            continue;
        }
        // So it comes to here if all data are valid.
        // d[0] is the vector from current pixel to the right one.
        // d[1] is the vector from current pixel to the below one.
        // Here we compute them.
        Vector3 d[2];
        for(int i=0;i<2;i++)
            for(int j=0;j<3;j++)
                d[i][j] = nb[i][j] - ss.value<float>(j);
        // The normal vector is the cross product of these two.
        Vector3 normal = normalize(cross(d[0],d[1]));
            sd.setPixel(normal.v);
    }

    // The last row, copy.
    ROIScanner sd1(norm,norm.height-1,0,norm.width-1,1);
    for(;sd1.isReady();sd1.next())
        for(int i=0;i<norm.dx;i++) // Pay attention to the int conversion. The
            sd1.ptr<float>()[i] = sd1.ptr<float>()[-(int)norm.dy+i]; // original
    // THe last column, copy.     dx/dy variables are uint, which can't do minus.
    ROIScanner sd2(norm,0,norm.width-1,1,norm.height);
    for(;sd2.isReady();sd2.next())
        for(int i=0;i<norm.dx;i++)
            sd2.ptr<float>()[i] = sd2.ptr<float>()[-(int)norm.dx+i];
    return norm;
}

// This function shade the fractal from a depth map and a normal map.
Image qLighting(Image surf,Image norm,qLightFieldParm &parm)
{
    Image canvas; canvas.emptyFrom(surf);
    // Setup three scanner
    // ss: the depth map.
    // sn: the normal map.
    // sc: the output image.
    ROIScanner ss(surf);
    ROIScanner sn(norm);
    ROIScanner sc(canvas);
    for(;ss.isReady();ss.next(),sn.next(),sc.next())
    {
        // Normal vector. For the empty pixel, the normal vector is zero.
        // So the output will be zero.
        Vector3 normal(sn.ptr<float>());
        // Compute the color using the Phong model.
        Vector3 color;
        // Compute the effect of each light one by one.
        for(int i=0;i<parm.lights.size();i++)
        {
            // Get the light.
            qLightParm light = parm.lights[i];
            // Compute the distance between the surface and the light.
            float dist = (light.position - Vector3(ss.ptr<float>())).norm1();
            // Unit vector from current surface to the light.
            Vector3 p2l = normalize(light.position - Vector3(ss.ptr<float>()));
            // Ambient
            color += light.ambient;
            // Diffuse
            color += light.diffuse * fabs(dot(p2l,normal))/(1+light.att*dist);
            // Specular(not implemented yet).
        }
        // Write the pixel.
        sc.setPixel(color.v);
    }
    return canvas;
}
