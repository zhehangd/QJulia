// File:   Image.h
// Author: Zhehang Ding
// Email:  dingzhehang1993@gmail.com
// Data:   Oct. 02, 2016

#ifndef IMAGE_H_
#define IMAGE_H_

#include <cstring>
#include <cstdint>

typedef int PixCoord;
typedef std::uint8_t  PixU8;
typedef std::uint16_t PixU16;
typedef std::uint32_t PixS32;
typedef float         PixF32;
typedef int           PixType;
typedef std::int64_t PixArthInt;
typedef float        PixArthFlt;

const int IMAGE_U8  = 1;
const int IMAGE_U16 = 2;
const int IMAGE_S32 = 4;
const int IMAGE_F32 = 4;


// This is a type-independent abstract of an image.
class ImageStruct
{
public:  
  // ------------------- Constructors/Deconstructors -------------------
  
  // Default constructor.
  ImageStruct(void);
  
  // Create a new image. See [create].
  ImageStruct(PixCoord w,PixCoord h,PixCoord ch,PixType elen){
    data=origin=0; copycount=0; create(w,h,ch,elen);}
    
  // Link to another image. See [link].
  ImageStruct(const ImageStruct& src){
    data=origin=0; copycount=0; link(src);}
    
  // Deconstructor.  
  ~ImageStruct(void){release();}
  
  // ------------------- Create/Link/Copy/Clone -------------------
  
  // Create a blank image given dimensions.
  // [w]:width, [h]:height, [ch]:#channel,  [elen]:#byte/pixel/channel
  void create(PixCoord w,PixCoord h,PixCoord ch,PixType elen);
  // Copy the image header from another image and share the content data.
  void link(const ImageStruct& src);
  // Copy the header and content from another image.
  void copy(const ImageStruct& src);
  // Make a independent clone of the image.
  ImageStruct  clone(void)const{ImageStruct dst;dst.copy(*this);return dst;}
  // Same as [link].
  ImageStruct& operator=(const ImageStruct& src){link(src);return *this;}
  
  
  // ------------------- Data Accessing -------------------
  
  // Test a coordinate.
  bool test(PixCoord r,PixCoord c,PixCoord ch=0)const{return r>=0 && r<height && c>=0 && c<width && ch>=0 && ch<channel;}
  
  // Obtain the one dimensional index of a pixel.
  PixCoord index(PixCoord r,PixCoord c,PixCoord ch=0)const{return r*cdy+c*cdx+ch*elength;}
  
  // Return a new image linking to a region of the image.
  ImageStruct roi(PixCoord r,PixCoord c,PixCoord w,PixCoord h)const;
  
  // Return the pointer corresponding to the given location.
  void* ptr(PixCoord r,PixCoord c,PixCoord ch=0)const{return (unsigned char*)data+index(r,c,ch);}
  
  // Return the pointer to the beginning of the given row.
  void* row(PixCoord r)const{return (unsigned char*)data+r*cdy;}
  
  // Access the pixel using "()".
  //template <class T>
  //T& operator()(PixCoord r,PixCoord c,PixCoord ch=0){return *(T*)ptr(r,c,ch);}
  //template <class T>
  //const T& operator()(PixCoord r,PixCoord c,PixCoord ch=0)const{return *(T*)ptr(r,c,ch);}
  
  // Dump a number of bytes into the given address.
  void dump(PixCoord r,PixCoord c,const void *src,unsigned int len){std::memcpy(ptr(r,c),src,len);}
  template <class T>
  void dump(PixCoord r,PixCoord c,const T& v){std::memcpy(ptr(r,c),&v,sizeof(T));}
  template <class T>
  void dump(PixCoord r,PixCoord c,const T& vr,const T& vg,const T& vb){T p[3]={vr,vg,vb};std::memcpy(ptr(r,c),&p,3*sizeof(T));}

  // Width and height of the ImageStruct
  PixType elength;
  PixCoord width;
  PixCoord height;
  PixCoord channel;
  PixCoord cdx;
  PixCoord cdy;

  char         *data;
  char         *origin;
  unsigned int *copycount;

  void allocate(PixCoord length);
  void release(void);
};

class ImCursor
{
public:
  ImCursor(void){cptr=0;cr=cc=ce=0;}
  ImCursor(const ImageStruct &image){set(image);}
  ImCursor(const ImageStruct &image,PixCoord x,PixCoord y,PixCoord w,PixCoord h){set(image.roi(y,x,w,h));}
  
  void set(const ImageStruct &image){imgst=image;cptr=imgst.data;cr=cc=ce=0;}
  void set(const ImageStruct &image,PixCoord x,PixCoord y,PixCoord w,PixCoord h){set(image.roi(y,x,w,h));}
  void restart(void){set(imgst);}
  
  // Move the cursor by the given row number.
  bool moveY(PixCoord y);//{cptr+=r*imgst.cdy;cr+=r;}
  // Move the cursor by the given col number
  bool moveX(PixCoord x);//{cptr+=c*imgst.cdx;cc+=c;}
  
  // Call moveR and moveC at the same time.
  bool moveXY(PixCoord x,PixCoord y);
  
  // At the last column.
  bool eoc(void){return cc==imgst.width;}
  // At the last row.
  bool eor(void){return cr==imgst.height;}
  // At the last pixel.
  bool eof(void){return eoc()&&eof();}
  
  // ---------------- Accessing ----------------
  
  // Get the data pointer at the current position.
  void* ptr(void){return cptr;}
  //
  template <class T>
  T& pixel(void)const{return *((T*)cptr);}
  template <class T>
  T& pixel(PixCoord i)const{return *((T*)cptr+i);}
  
  void dump(const void *src,unsigned int len){std::memcpy(cptr,src,len);}
  template <class T>
  void dump(const T& v){std::memcpy(cptr,&v,sizeof(T));}
  template <class T>
  void dump(const T& vr,const T& vg,const T& vb){T p[3]={vr,vg,vb};std::memcpy(cptr,&p,3*sizeof(T));}

  bool moveNext(void);
  bool moveNextElement(void);
  
  char* cptr;
  ImageStruct imgst;
  PixCoord  cr,cc,ce;
};


class Image: public ImageStruct
{
public:
  Image(void):ImageStruct(){}
  Image(PixCoord w,PixCoord h,PixCoord ch,uint8_t type):ImageStruct(w,h,ch,type){}
  Image(const Image& src):ImageStruct(src){}
  Image(const ImageStruct& src):ImageStruct(src){}
  
  Image roi(PixCoord r,PixCoord c,PixCoord w,PixCoord h)const{return Image(ImageStruct::roi(r,c,w,h));}
  bool  interp(float r,float c,void *dst)const;
  
  Image operator+(PixArthInt c)const{Image image = this->clone();return image+=c;}
  Image operator-(PixArthInt c)const{Image image = this->clone();return image-=c;}
  Image operator*(PixArthInt c)const{Image image = this->clone();return image*=c;}
  Image operator/(PixArthInt c)const{Image image = this->clone();return image/=c;}
  
  Image& operator+=(PixArthInt c);
  Image& operator-=(PixArthInt c);
  Image& operator*=(PixArthInt c);
  Image& operator/=(PixArthInt c);
  
  Image& operator+=(const Image& c);
  Image& operator-=(const Image& c);
  Image& operator*=(const Image& c);
  Image& operator/=(const Image& c);
  
  Image operator+(const Image& c)const{Image image = this->clone();return image+=c;}
  Image operator-(const Image& c)const{Image image = this->clone();return image-=c;}
  Image operator*(const Image& c)const{Image image = this->clone();return image*=c;}
  Image operator/(const Image& c)const{Image image = this->clone();return image/=c;}
};


class ImFloat: public ImageStruct
{
public:
  ImFloat(void):ImageStruct(){}
  ImFloat(PixCoord w,PixCoord h,PixCoord ch,uint8_t type=IMAGE_F32):ImageStruct(w,h,ch,type){}
  ImFloat(const ImFloat& src):ImageStruct(src){}
  ImFloat(const ImageStruct& src):ImageStruct(src){}
  
  ImFloat roi(PixCoord r,PixCoord c,PixCoord w,PixCoord h)const{return ImFloat(ImageStruct::roi(r,c,w,h));}
  
  ImFloat operator+(PixArthFlt c)const{ImFloat image = this->clone();return image+=c;}
  ImFloat operator-(PixArthFlt c)const{ImFloat image = this->clone();return image-=c;}
  ImFloat operator*(PixArthFlt c)const{ImFloat image = this->clone();return image*=c;}
  ImFloat operator/(PixArthFlt c)const{ImFloat image = this->clone();return image/=c;}
  
  ImFloat& operator+=(PixArthFlt c);
  ImFloat& operator-=(PixArthFlt c);
  ImFloat& operator*=(PixArthFlt c);
  ImFloat& operator/=(PixArthFlt c);
  
  ImFloat& operator+=(const ImFloat& c);
  ImFloat& operator-=(const ImFloat& c);
  ImFloat& operator*=(const ImFloat& c);
  ImFloat& operator/=(const ImFloat& c);
  
  ImFloat operator+(const ImFloat& c)const{ImFloat image = this->clone();return image+=c;}
  ImFloat operator-(const ImFloat& c)const{ImFloat image = this->clone();return image-=c;}
  ImFloat operator*(const ImFloat& c)const{ImFloat image = this->clone();return image*=c;}
  ImFloat operator/(const ImFloat& c)const{ImFloat image = this->clone();return image/=c;}
};

Image imconvert(const Image& src,PixType elen);
Image imconvert(const ImFloat& src,PixType elen);
ImFloat imconvert2float(const Image& src);

bool imwrite(const ImFloat &image,const char* filename);
bool imwrite(const Image &image,const char* filename);
bool imread(const char* filename, Image &image);


void iminfo(ImageStruct &image);



#endif


  // Here is a memo for myself about rule of copy and assignment.
  // 1. A derived class constructor must call the base class constructor. 
  //      If it is not explicitly called, then the (void) one is called.
  // 2. Function calling priority: derived class > base class > derived default
  // 3. Derived objects can not assign to a base object.
  //      However, in case of the derived-from-base constructor is defined,
  //      There is a implicit conversion then the assignment.
  //      But in this case, operator= is still needed, otherwise the
  //      the copy-counter would not work.
  













