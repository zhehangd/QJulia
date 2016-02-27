// File:   Image.cpp
// Author: Zhehang Ding
// Email:  dingzhehang1993@gmail.com
// Data:   Feb. 27, 2016

// C++11 round() is used.
#include <cmath>
#include <iostream>
#include <fstream>

#include "Image.h"


void Image::create(unsigned int w,unsigned int h,unsigned int ch,int t)
{
    //
    width   = w;
    height  = h;
    type    = t;
    channel = ch;
    
    // 
    if(type==IMAGE_INT)
        eLen = sizeof(int);
    else
        if(type==IMAGE_FLOAT)
            eLen = sizeof(float);
        else
            eLen = sizeof(char);
    dx = channel;
    dy = dx*width;
    c_dx = dx * eLen;
    c_dy = dy * eLen;
    size = width*height;
    numel = size*channel;
    mSize = numel*eLen;
    
    // Allocate memory and initialize it.
    allocate(mSize);
    for(unsigned int i=0;i<mSize;i++)
        data[i] = 0;
}



void Image::allocate(unsigned int length)
{
    // First release the old data.
    release();
    // Allocate memory.
    data = new char[length];
    // Allocate memory for counter variable and assign 1 to it.
    copycount = new unsigned int(1);
}

void Image::clear(void)
{
    width = height = 0;
    type  = channel = 0;
    eLen  = mSize = 0;
    dx    = dy = 0;
    c_dx  = c_dy = 0;
    size  = numel = 0;
    release();
}

void Image::release(void)
{
    // This method first checks the pointer,
    // if there is no memory allocated, it
    // does nothing, otherwise it subtracts 1
    // from the integer pointed to by [copycount].
    // Then it checks its value. If the counter reaches
    // zero, it means there is no Image object pointing
    // to this block any more, thus it deallocates the block
    // and sets all variables to zero.
    if(data!=0)
    {
        (*copycount)--;
        if(*copycount==0)
        {
            delete[] data;
            delete copycount;
            data = 0;
            copycount = 0;
        }
    }
}

void Image::emptyFrom(const Image& src)
{
    create(src.width,src.height,src.channel,src.type);
}

void Image::copyTo(Image& dst)const
{
    dst.create(width,height,channel,type);
    for(unsigned int i=0;i<dst.mSize;i++)
        dst.data[i] = data[i];
}

void Image::assign(Image& dst)const
{
    dst.width     = width;
    dst.height    = height;
    dst.channel   = channel;
    dst.dx        = dx;
    dst.dy        = dy;
    dst.c_dx      = c_dx;
    dst.c_dy      = c_dy;
    dst.type      = type;
    dst.eLen      = eLen;
    dst.mSize     = mSize;
    dst.size      = size;
    dst.numel     = numel;
    dst.data      = data;
    dst.copycount = copycount;
    if(copycount!=0)
        (*dst.copycount)++;
}

Image Image::clone(void)const
{
    Image image;
    copyTo(image);
    return image;
}

// 
void* Image::getPixelPtr(unsigned int r,unsigned int c)const
{
    // [ASSERT]
    return data+r*c_dy+c*c_dx;
}


Image Image::convert(int dstType)
{
    Image dst;
    if(type==IMAGE_FLOAT)
    {
        if(type==dstType)
            dst = clone();
        else
        {
            dst.create(width,height,channel,dstType);
            if(dstType==IMAGE_INT)
                for(unsigned int i=0;i<numel;i++)
                    ((int*)dst.data)[i] = ((float*)data)[i];
            else
                for(unsigned int i=0;i<numel;i++)
                {
                    float v = ((float*)data)[i];v = v>255?255:v>0?v:0;
                    ((unsigned char*)dst.data)[i] = (unsigned char)v;
                }
        }
    }
    else
    {
        if(type==IMAGE_INT)
        {
            if(type==dstType)
                dst = clone();
            else
            {
                dst.create(width,height,channel,dstType);
                if(dstType==IMAGE_FLOAT)
                    for(unsigned int i=0;i<numel;i++)
                        ((float*)dst.data)[i] = ((int*)data)[i];
                else
                    for(unsigned int i=0;i<numel;i++)
                    {
                        int v = ((int*)data)[i]; v = v>255?255:v>0?v:0;
                        ((unsigned char*)dst.data)[i] = v;
                    }
            }
        }
        else
        {
            if(dstType==IMAGE_FLOAT ||  dstType==IMAGE_INT)
            {
                dst.create(width,height,channel,dstType);
                if(dstType==IMAGE_FLOAT)
                    for(unsigned int i=0;i<numel;i++)
                        ((float*)dst.data)[i] = ((unsigned char*)data)[i];
                else
                    for(unsigned int i=0;i<numel;i++)
                        ((int*)dst.data)[i] = ((unsigned char*)data)[i];
            }
            else
                dst = clone();
        }
    }
    return dst;
}


// Open a raw image
Image& Image::read(const char* filename,
              unsigned int w,unsigned int h,unsigned int ch,int type)
{
    // Open the file.
    std::ifstream file;
    file.exceptions(std::ifstream::failbit|std::ifstream::failbit);
    file.open(filename,std::ios::binary);
    
    // Create a new image.
    create(w,h,ch,type);
    
    // Read image data, the amount depends on the given size.
    // Trying to read more data than the file contains cause an error,
    // but read less data is allowable.
    file.read(data,mSize);

    return *this;
}

// Write the image to the given file.
void Image::write(const char* filename)
{
    std::ofstream file;
    file.exceptions(std::ifstream::failbit|std::ifstream::failbit);
    file.open(filename,std::ostream::binary);
    file.write(data,mSize);
}


void Image::show(int r,int c,int w,int h)
{
    for(ROIScanner scan(*this,r,c,w,h);scan.isReady();scan.next())
    {
        if(scan.isRowChanged())
           std::cout<<"\n";
        std::cout<<"(";
        switch(type)
        {
            
            case IMAGE_FLOAT:
                for(unsigned int i=0;i<1;i++)
                    std::cout<<*scan.ptr<float>(i)<<",";
                break;    
                
            case IMAGE_INT:
                for(unsigned int i=0;i<1;i++)
                    std::cout<<*scan.ptr<int>(i)<<",";
                break;
            default:
                for(unsigned int i=0;i<1;i++)
                    std::cout<<(int)*scan.ptr<unsigned char>(i)<<",";
                break;
        }
        std::cout<<") ";
    }
    std::cout<<"\n-----------------------------\n";
}

Image Image::operator+(int c)const
{
    if(type==IMAGE_FLOAT)
        return *this + (float)c;
    if(type==IMAGE_INT)
    {
        Image dst = this->clone();
        int *ptr = (int*)dst.getPixelPtr(0,0);
        for(unsigned int i=0;i<numel;i++)
            ptr[i] += c;
        return dst;
    }
    else
    {
        Image dst = this->clone();
        char *ptr = (char*)dst.getPixelPtr(0,0);
        for(unsigned int i=0;i<numel;i++)
        {
            int v = ptr[i] + c;
                v = v>255?v:v<0?0:v;
            ptr[i] = v;
        }
        return dst;
    }
}

Image Image::operator-(int c)const
{
    return *this + (-c);
}

Image Image::operator*(int c)const
{
    if(type==IMAGE_FLOAT)
        return *this * (float)c;
    if(type==IMAGE_INT)
    {
        Image dst = this->clone();
        int *ptr = (int*)dst.getPixelPtr(0,0);
        for(unsigned int i=0;i<numel;i++)
            ptr[i] *= c;
        return dst;
    }
    else
    {
        Image dst = this->clone();
        char *ptr = (char*)dst.getPixelPtr(0,0);
        for(unsigned int i=0;i<numel;i++)
        {
            int v = ptr[i] * c;
                v = v>255?v:v<0?0:v;
            ptr[i] = v;
        }
        return dst;
    }
}

Image Image::operator/(int c)const
{
    if(type==IMAGE_FLOAT)
        return *this + (float)c;
    if(type==IMAGE_INT)
    {
        Image dst = this->clone();
        int *ptr = (int*)dst.getPixelPtr(0,0);
        for(unsigned int i=0;i<numel;i++)
            ptr[i] /= c;
        return dst;
    }
    else
    {
        Image dst = this->clone();
        char *ptr = (char*)dst.getPixelPtr(0,0);
        for(unsigned int i=0;i<numel;i++)
        {
            int v = ptr[i] / c;
                v = v>255?v:v<0?0:v;
            ptr[i] = v;
        }
        return dst;
    }
    return *this;
}

//#define ROUND(c) std::round(((c)>0?(c)+0.5f:(c)-0.5f))

Image Image::operator+(float c)const
{
    if(type==IMAGE_FLOAT)
    {
        Image dst = this->clone();
        float *ptr = (float*)dst.getPixelPtr(0,0);
        for(unsigned int i=0;i<numel;i++)
            ptr[i] += c;
        return dst;
    }
    else
        return *this + (int)std::round(c);
}

Image Image::operator-(float c)const
{
    return *this + (-c);
}

Image Image::operator*(float c)const
{
    Image dst = this->clone();
    switch(type)
    {
        case IMAGE_FLOAT:
        {
            float *ptr = (float*)dst.getPixelPtr(0,0);
            for(unsigned int i=0;i<numel;i++)
                ptr[i] *= c;
            break;
        }
        case IMAGE_INT:
        {
            int *ptr = (int*)dst.getPixelPtr(0,0);
            for(unsigned int i=0;i<numel;i++)
                ptr[i] = std::round((float)ptr[i]*c);
            break;
        }
         default:
        {
            char *ptr = (char*)dst.getPixelPtr(0,0);
            for(unsigned int i=0;i<numel;i++)
            {
                int v = std::round((float)ptr[i]*c);
                    v = v>255?v:v<0?0:v;
                ptr[i] *= c;
            }
            break;
        }
    }
    return dst;
}

Image Image::operator/(float c)const
{
    Image dst = this->clone();
    switch(type)
    {
        case IMAGE_FLOAT:
        {
            float *ptr = (float*)dst.getPixelPtr(0,0);
            for(unsigned int i=0;i<numel;i++)
                ptr[i] /= c;
            break;
        }
        case IMAGE_INT:
        {
            int *ptr = (int*)dst.getPixelPtr(0,0);
            for(unsigned int i=0;i<numel;i++)
                ptr[i] = std::round((float)ptr[i]/c);
            break;
        }
         default:
        {
            char *ptr = (char*)dst.getPixelPtr(0,0);
            for(unsigned int i=0;i<numel;i++)
            {
                int v = std::round((float)ptr[i]*c);
                    v = v>255?v:v<0?0:v;
                ptr[i] /= c;
            }
            break;
        }
    }
    return dst;
}



void ROIScanner::setup(const Image &image,int r,int c,int w,int h)
{
    optr = (char*)image.getPixelPtr(r,c);
    cptr = optr;
    regR = r;
    regC = c;
    regW = w;
    regH = h;
    cntR = 0;
    cntC = 0;
    c_dx = image.c_dx;
    c_dy = image.c_dy;
    isSetupFlag = true;
    isReadyFlag = true;
    isRowChangeFlag = false;
}

void ROIScanner::restart(void)
{
    cptr = optr;
    cntR = 0;
    cntC = 0;
    isSetupFlag = isSetupFlag;  // In case being called without setup.
    isReadyFlag = true;
    isRowChangeFlag = false;
}

void ROIScanner::setup(const Image &image)
{
    optr = image.data;
    cptr = optr;
    regR = 0;
    regC = 0;
    regW = image.width;
    regH = image.height;
    cntR = 0;
    cntC = 0;
    c_dx   = image.c_dx;
    c_dy   = image.c_dy;
    isReadyFlag = true;
    isRowChangeFlag = false;
}


bool ROIScanner::next(void)
{
    // If the current state is not valid, return
    if(isReadyFlag==false)
        return false;
    isRowChangeFlag = false;// The row change flag only set for one turn.
    cntC++;                 // Increase the column counter.
    cptr += c_dx;           // Increase the pointer.
    if(cntC<regW)           // If the right boundary is not met.
        return true;
    cntC = 0;               // Run to here when reach the boundary
    cntR++;                 // Increase the row counter.
    cptr += c_dy-c_dx*regW; // Rewind the pointer.
    isRowChangeFlag = true; // Set the row change flag.
    if(cntR<regH)           // Check if comes to the end of the region.
        return true;
    isReadyFlag = false;    // To here when it ends.
    return false;
}
