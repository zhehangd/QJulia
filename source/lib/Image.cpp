// File:   Image.cpp
// Author: Zhehang Ding
// Email:  dingzhehang1993@gmail.com
// Data:   Oct. 02, 2016

#include "Image.hpp"
#include <climits>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>
#include <cstdio>
#include <cmath>

using namespace std;

ImageBase::ImageBase(void)
{
  data=0; origin=0; copycount=0;
  width=height=channel=0;
  elength=cdx=cdy=0;
}

void ImageBase::create(int w,int h,int ch,int elen)
{
  width   = w;
  height  = h;
  channel = ch;
  elength = elen;
  cdx     = channel*elength;
  cdy     = cdx*width;
  release();
  allocate(width*height*channel*elength);
}

void ImageBase::link(const ImageBase& src)
{
  if (&src == this)
    return;
  release();
  width   = src.width;
  height  = src.height;
  channel = src.channel;
  elength = src.elength;
  cdx     = src.cdx;
  cdy     = src.cdy;
  data    = src.data;
  origin  = src.origin;
  copycount = src.copycount;
  if(copycount!=0)
    (*copycount)++;
  //cout<<"["<<(int*)origin<<"] "<<"Increase to "<<(*copycount)<<endl;
}

void ImageBase::copy(const ImageBase& src)
{
  if (&src == this)
    return;
  create(src.width,src.height,src.channel,src.elength);
  for(int r=0;r<height;r++)
    std::memcpy(row(r),src.row(r),src.cdx*src.width);
}

ImageBase ImageBase::roi(int r ,int c ,int w ,int h )const
{
  ImageBase dst(*this);
  dst.width  = w;
  dst.height = h;
  dst.data  += index(r,c);
  return dst;
}

void ImageBase::allocate(int length)
{
  origin = data = new char[length];
  copycount  = new unsigned int(1);
  //cout<<"["<<(int*)origin<<"] "<<" Allocate "<<length<<endl;
}

void ImageBase::release(void)
{
  if(copycount!=0){
    //cout<<"["<<(int*)origin<<"] "<<"Attemp to release "<<flush;
    (*copycount)--;
    //cout<<"  Minus.."<<*copycount<<".."<<flush;
    if(*copycount==0){
      delete[] origin;
      delete copycount;
      //cout<<"Approved.."<<flush;
    }
    //cout<<endl;
  }
  
  data=0; origin=0; copycount=0;
}


bool ImCursor::moveX(int x)
{
  if (cc+x>=imgst.width || cc-x<0)
    return false;
  cptr = cptr + x*imgst.cdx;;
  cc   = cc   + x;
  return true;
}

bool ImCursor::moveY(int y)
{
  if (cr+y>=imgst.height || cr-y<0)
    return false;
  cptr = cptr + y*imgst.cdy;;
  cr   = cr   + y;
  return true;
}

bool ImCursor::moveXY(int x,int y)
{
  bool flag = false;
  flag |= cc+x>=imgst.width  || cc-x<0;
  flag |= cr+y>=imgst.height || cr-y<0;
  if (flag==true)
    return false;
  cptr = cptr + x*imgst.cdx;;
  cc   = cc   + x;
  cptr = cptr + y*imgst.cdy;;
  cr   = cr   + y;
}

bool ImCursor::moveNext(void)
{
  if (!imgst.test(cr,cc))
    return false;
  cptr+= imgst.cdx;
  cc  += 1;
  if(cc<imgst.width)
    return true;
  cc   = 0;
  cr  += 1;
  cptr = (char*)imgst.ptr(cr,0);
  return true;
}

bool ImCursor::moveNextElement(void)
{
  if (cc==(imgst.width-1) && cr==(imgst.height-1) && ce==(imgst.channel-1))
    return false;
  cptr += imgst.elength;
  ce   += 1;
  if (ce<imgst.channel)
    return true;
  ce  = 0;
  cc += 1;
  if(cc<imgst.width)
    return true;
  cc   = 0;
  cr  += 1;
  cptr = (char*)imgst.ptr(cr,0);
  return true;
}

template<class T>
void ImOperationInt(Image &image,PixArthInt c,PixArthInt (*func)(PixArthInt,PixArthInt))
{
  numeric_limits<T> limits;
  for(int i=0;i<image.height;i++)
  {
    T* ptr = (T*)image.row(i);
    for(int i=0;i<image.width*image.channel;i++)
    {
      PixArthInt v = func((PixArthInt)ptr[i],c);
      if (v>(PixArthInt)limits.max()){
        v = limits.max();}
      if (v<(PixArthInt)limits.min()){
        v = limits.min();}
      ptr[i] = (T)(v);
    }
  }
}

template<class T>
void ImOperationInt(Image &image,const Image &src,PixArthInt (*func)(PixArthInt,PixArthInt))
{
  bool flag = true;
  flag &= image.width   <= src.width;
  flag &= image.height  <= src.height;
  flag &= image.channel == src.channel;
  flag &= image.elength == src.elength;
  if ( !flag )
    return;
  numeric_limits<T> limits;
  for(int i=0;i<image.height;i++)
  {
    T* pdst = (T*)image.row(i);
    T* psrc = (T*)src.row(i);
    for(int i=0;i<image.width*image.channel;i++)
    {
      PixArthInt v = func((PixArthInt)pdst[i],(PixArthInt)psrc[i]);
      if (v>(PixArthInt)limits.max()){
        v = limits.max();}
      if (v<(PixArthInt)limits.min()){
        v = limits.min();}
      pdst[i] = (T)(v);
    }
  }
}

#define OPERATION_INT_TEMPLATE(FUNC) \
switch(elength){                 \
  case IMAGE_U8 : ImOperationInt<PixU8>( *this,c,(FUNC));break; \
  case IMAGE_U16: ImOperationInt<PixU16>(*this,c,(FUNC));break; \
  case IMAGE_S32: ImOperationInt<PixS32>(*this,c,(FUNC));break; \
} return *this;                                                     \

PixArthInt ImOpIntAdd(PixArthInt a,PixArthInt b){return a+b;}
PixArthInt ImOpIntSub(PixArthInt a,PixArthInt b){return a-b;}
PixArthInt ImOpIntMul(PixArthInt a,PixArthInt b){return a*b;}
PixArthInt ImOpIntDiv(PixArthInt a,PixArthInt b){return a/b;}

Image& Image::operator+=(PixArthInt c){OPERATION_INT_TEMPLATE(ImOpIntAdd)}
Image& Image::operator-=(PixArthInt c){OPERATION_INT_TEMPLATE(ImOpIntSub)}
Image& Image::operator*=(PixArthInt c){OPERATION_INT_TEMPLATE(ImOpIntMul)}
Image& Image::operator/=(PixArthInt c){OPERATION_INT_TEMPLATE(ImOpIntDiv)}

Image& Image::operator+=(const Image& c){OPERATION_INT_TEMPLATE(ImOpIntAdd)}
Image& Image::operator-=(const Image& c){OPERATION_INT_TEMPLATE(ImOpIntSub)}
Image& Image::operator*=(const Image& c){OPERATION_INT_TEMPLATE(ImOpIntMul)}
Image& Image::operator/=(const Image& c){OPERATION_INT_TEMPLATE(ImOpIntDiv)}



template<class T>
void ImOperationFlt(ImFloat &image,PixArthFlt c,PixArthFlt (*func)(PixArthFlt,PixArthFlt))
{
  numeric_limits<T> limits;
  for(int i=0;i<image.height;i++)
  {
    T* ptr = (T*)image.row(i);
    for(int i=0;i<image.width*image.channel;i++)
    {
      PixArthFlt v = func((PixArthFlt)ptr[i],c);
      ptr[i] = (T)(v);
    }
  }
}

template<class T>
void ImOperationFlt(ImFloat &image,const ImFloat &src,PixArthFlt (*func)(PixArthFlt,PixArthFlt))
{
  bool flag = true;
  flag &= image.width   <= src.width;
  flag &= image.height  <= src.height;
  flag &= image.channel == src.channel;
  flag &= image.elength == src.elength;
  if ( !flag )
    return;
  numeric_limits<T> limits;
  for(int i=0;i<image.height;i++)
  {
    T* pdst = (T*)image.row(i);
    T* psrc = (T*)src.row(i);
    for(int i=0;i<image.width*image.channel;i++)
    {
      PixArthFlt v = func((PixArthFlt)pdst[i],(PixArthFlt)psrc[i]);
      pdst[i] = (T)(v);
    }
  }
}

#define OPERATION_FLT_TEMPLATE(FUNC) \
switch(elength){                     \
  case IMAGE_F32 : ImOperationFlt<PixF32>( *this,c,(FUNC));break; \
} return *this;                                                  \

PixArthFlt ImOpFltAdd(PixArthFlt a,PixArthFlt b){return a+b;}
PixArthFlt ImOpFltSub(PixArthFlt a,PixArthFlt b){return a-b;}
PixArthFlt ImOpFltMul(PixArthFlt a,PixArthFlt b){return a*b;}
PixArthFlt ImOpFltDiv(PixArthFlt a,PixArthFlt b){return a/b;}

ImFloat& ImFloat::operator+=(PixArthFlt c){OPERATION_FLT_TEMPLATE(ImOpFltAdd)}
ImFloat& ImFloat::operator-=(PixArthFlt c){OPERATION_FLT_TEMPLATE(ImOpFltSub)}
ImFloat& ImFloat::operator*=(PixArthFlt c){OPERATION_FLT_TEMPLATE(ImOpFltMul)}
ImFloat& ImFloat::operator/=(PixArthFlt c){OPERATION_FLT_TEMPLATE(ImOpFltDiv)}

ImFloat& ImFloat::operator+=(const ImFloat& c){OPERATION_FLT_TEMPLATE(ImOpFltAdd)}
ImFloat& ImFloat::operator-=(const ImFloat& c){OPERATION_FLT_TEMPLATE(ImOpFltSub)}
ImFloat& ImFloat::operator*=(const ImFloat& c){OPERATION_FLT_TEMPLATE(ImOpFltMul)}
ImFloat& ImFloat::operator/=(const ImFloat& c){OPERATION_FLT_TEMPLATE(ImOpFltDiv)}


template<class Ts,class Td>
ImageBase ImConvertTemplate(const ImageBase &src)
{
  Image dst(src.width,src.height,src.channel,sizeof(Td));
  numeric_limits<Td> limits;
  for(int i=0;i<dst.height;i++)
  {
    Td* pdst = (Td*)dst.row(i);
    Ts* psrc = (Ts*)src.row(i);
    for(int i=0;i<dst.width*dst.channel;i++)
    {
      Ts vs = psrc[i];
      //printf("%5.2f ",vs);
      Td vd = vs;
      if (vs>limits.max()){
        vd = limits.max();}
      if (vs<limits.min()){
        vd = limits.min();}
      pdst[i] = vd;
    }
    //printf("\n");
  }
  return dst;
}



ImFloat imconvert2float(const Image& src)
{
  ImFloat dst;
  switch(src.elength)
  {
    case IMAGE_U8:  dst=ImConvertTemplate<PixU8,PixF32>(src); break;
    case IMAGE_U16: dst=ImConvertTemplate<PixU16,PixF32>(src);break;
    case IMAGE_F32: dst=ImConvertTemplate<PixS32,PixF32>(src);break;
  }
  return dst;
}

Image imconvert(const ImFloat& src,int elen)
{
  Image dst;
  switch(elen)
  {
    case IMAGE_U8:  dst=ImConvertTemplate<PixF32,PixU8>(src); break;
    case IMAGE_U16: dst=ImConvertTemplate<PixF32,PixU16>(src);break;
    case IMAGE_S32: dst=ImConvertTemplate<PixF32,PixS32>(src);break;
  }
  return dst;
}

Image imconvert(const Image& src,int elen)
{
  if (src.elength==elen)
    return src.clone();
  Image dst;
  switch(src.elength)
  {
    case IMAGE_U8:
      switch(elen){
        case IMAGE_U16: dst=ImConvertTemplate<PixU8,PixU16>(src);  break;
        case IMAGE_S32: dst=ImConvertTemplate<PixU8,PixS32>(src);  break;
      }break;
    case IMAGE_U16:
      switch(elen){
        case IMAGE_U8:  dst=ImConvertTemplate<PixU16,PixU8>(src);  break;
        case IMAGE_S32: dst=ImConvertTemplate<PixU16,PixS32>(src); break;
      }break;
    case IMAGE_S32:
      switch(elen){
        case IMAGE_U8:  dst=ImConvertTemplate<PixS32,PixU8>(src); break;
        case IMAGE_U16: dst=ImConvertTemplate<PixS32,PixU16>(src); break;
      }break;    
  }
  return dst;
}

bool imwrite(const ImFloat &image,const char* filename)
{
  return imwrite(imconvert(image*255,1),filename); 
}

bool imwrite(const Image &image,const char* filename)
{
  if (image.channel!=1 && image.channel!=3)
    return false;
  
  Image output = imconvert(image,1);
  std::ostringstream ss;
  if ( image.channel == 3)
    ss<<"P6 ";
  else
    ss<<"P5 ";
  ss<<output.width<<" "<<output.height<<" 255\r";
  
  std::ofstream file;
  file.exceptions(std::ofstream::failbit|std::ofstream::failbit);
  file.open(filename,std::ostream::binary);
  
  file.write(ss.str().c_str(),ss.str().size());
  for(int i=0;i<output.height;i++)
    file.write((char*)output.ptr(i,0),output.cdx*output.width);
  
  file.close();
  return true;
}

bool imread(const char* filename, Image &image)
{
  // Assume the following format.
  // ----------------------------
  // [P5] or [P6].
  // # One or more possible comments.
  // [Width] [Height] [MAX]
  // [Raster]
  // ----------------------------
  int width,height,vmax,ch;

  std::ifstream file;
  file.open(filename,std::istream::binary);
  if(file.is_open()==false)
    return false;

  // The first char should be 'P'.
  if(file.get()!='P')
    return false;
  // Read the format and parse the number of channel.
  // Only P5 and P6 are supported.
  ch = file.get();
  ch = (ch=='6')?3:((ch=='5')?1:0);
  if(ch==0)
    return false;
  if (std::isspace(file.get())==false)
    return false;

  // Skip spaces and comments.
  while(std::isspace(file.peek()))
    file.ignore();
  while( file.peek() == '#' ){
    char c = file.get();
    while( c!='\n' && c!='\r' )
      c = file.get();
  }
  
  
  file >> width >> height >> vmax;
  
  if (std::isspace(file.get())==false)
    return false;
  
  // Create the image.
  if(width<=0 || height<=0 || vmax<=0)
    return false;

  Image input(width,height,ch,1);
  
  cout<<width<<" "<<width<<" "<<height<<" "<<input.cdx<<endl;
  
  file.read((char*)input.ptr(0,0),width*height*input.cdx);
  if(file.eof()==true || file.good()==false)
    return false;
  file.close();
  
  image = input;
  return true;
}

void iminfo(ImageBase &image)
{
  cout<<" --------- Image Info --------- "<<endl;
  cout<<" data   : "<<(uint32_t*)image.data<<endl;
  cout<<" address: "<<(uint32_t*)image.origin<<endl;
  cout<<" elength:"  <<image.elength<<endl;
  cout<<" width:  "  <<image.width<<endl;
  cout<<" height: "  <<image.height<<endl;
  cout<<" channel:"  <<image.channel<<endl;
  cout<<" cdx:    "  <<image.cdx<<endl;
  cout<<" cdy:    "  <<image.cdy<<endl;
  cout<<" ----------------------------- "<<endl;
}

template<class T>
bool tImageInterp(const Image &image,float r,float c,void *dst)
{
  if (image.test(r,c)==false)
    return false;
  int fr = std::floor(r);
  int fc = std::floor(c);
  int cr = std::ceil(r);
  int cc = std::ceil(c);
  float ar = r - std::floor(r);
  float ac = c - std::floor(c);

  if((fr==(image.height-1))&&(fc==(image.width-1)))
  {
    T* p1 = (T*)image.ptr(fr,fc);
    T* p  = (T*)dst;
    for(int i=0;i<image.channel;i++)
      p[i] = p1[i];
    return true;
  }
  
  if(fr==(image.height-1))
  {
    T* p1 = (T*)image.ptr(fr,fc);
    T* p2 = (T*)image.ptr(fr,cc);
    T* p  = (T*)dst;
    for(int i=0;i<image.channel;i++)
      p[i] = (1-ac)*p1[i] + ac*p2[i];
    return true;
  }
  
  if(fc==(image.width-1))
  {
    T* p1 = (T*)image.ptr(fr,fc);
    T* p3 = (T*)image.ptr(fr,cc);
    T* p  = (T*)dst;
    for(int i=0;i<image.channel;i++)
      p[i] = (1-ar)*p1[i] + ar*p3[i];
    return true;
  }
  
  if(fr==(image.height-1))
  {
    T* s = (T*)image.ptr(fr,fc);
    T* p = (T*)dst;
    for(int i=0;i<image.channel;i++)
      p[i] = s[i];
  }
  
  T* p1 = (T*)image.ptr(fr,fc);
  T* p2 = (T*)image.ptr(fr,cc);
  T* p3 = (T*)image.ptr(cr,fc);
  T* p4 = (T*)image.ptr(cr,cc);
  
  float *v1 = new float[image.channel];
  float *v2 = new float[image.channel];
  float *v3 = new float[image.channel];
  for(int i=0;i<image.channel;i++)
  {
    v1[i] = (1-ac)*p1[i] + ac*p2[i];
    v2[i] = (1-ac)*p3[i] + ac*p4[i];
    v3[i] = (1-ar)*v1[i] + ar*v2[i];
  }

  T* p = (T*)dst;
  for(int i=0;i<image.channel;i++)
    p[i] = v3[i];

  return true;
}

bool Image::interp(float r,float c,void *dst)const
{
  switch(elength)
  {
  case IMAGE_U8:
    return tImageInterp<PixU8>(*this,r,c,dst);
  case IMAGE_U16:
    return tImageInterp<PixU16>(*this,r,c,dst);
  case IMAGE_S32:
    return tImageInterp<PixS32>(*this,r,c,dst);
  }
  return false;
}


#if 0

#include <cmath>
using namespace std;

int main(void)
{

  ImFloat imjulia(640,480,3,sizeof(float));
  
  for(int j=0;j<480;j++)
  {
    for(int i=0;i<640;i++)
    {
      double a=0,b=0,c,d,n=0;
      while( (c=a*a)+(d=b*b)<4 && n++<880 )
      {
        b=2*a*b + j*2e-8 -.645411;
        a=  c-d + i*2e-8 +.356888;
      }
      float *p = (float*)imjulia.ptr(j,i);
      
      p[0] = pow((n-80)/800,.7);
      p[1] = pow((n-80)/800,.5);
      p[2] = pow((n-80)/800,.3);
    }
  }
  
  ImCursor scan(imjulia,10,40,100,200);
  do
  {
    scan.dump<float>(0.2,0.9,0.9);
  }while(scan.moveNext());
  

  Image image = imconvert(imjulia*255,1);

  
  Image roi1 = image.roi(150,100,200,100);
  Image roi2 = image.roi(200,200,200,100);
  roi1 /= 3;
  roi1 += roi2;
  //roi2 *= 2;
  
  iminfo(image);
  iminfo(roi1);
  iminfo(roi2);
  
  imwrite(image,"canvas.ppm");
  imwrite(roi1,"roi1.ppm");
  imwrite(roi2,"roi2.ppm");
}

#endif



