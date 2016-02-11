// File:   Image.h
// Author: Zhehang Ding
// Email:  dingzhehang1993@gmail.com
// Data:   Feb. 09, 2016

#ifndef IMAGE_H_
#define IMAGE_H_

// This is the new edition of my old Image class deveoped when I took
// EE569 in USC, 2015 Fall.

#define IMAGE_CHAR  (0x00)
#define IMAGE_INT   (0x01)
#define IMAGE_FLOAT (0x02)

class Image
{
public:
    // Default constructor.
    Image(void){data=0;copycount=0;clear();}
    // Create a new image given its parameter, see create().
    Image(unsigned int w,unsigned int h,unsigned int ch,int type)
                                {data=0;copycount=0;create(w,h,ch,type);}
    // Copy the header of an existed image, see assign().
    Image(const Image& src){data=0;copycount=0;src.assign(*this);}
    // Destructor.
    ~Image(void){release();}
    
    //====================== I/O Methods ======================
    // Delete the image memory and information.
    void clear(void);
    

    // Create a new blank image
    // [w] and [h] are width and height respectively, do not reverse it. 
    // [ch] is the number of channels.
    // [fill] denotes the constant value the image initially should has.
    void create(unsigned int w,unsigned int h,unsigned int ch,int type);

    void create(const Image& image);
    
    Image convert(int dstType);
    
    // Open a raw image, set up the object. [w] and [h] are width and height of the image,
    // [ch] is the number of channels of the image.
    Image& read(const char* filename,unsigned int w,unsigned int h,unsigned int ch,int type=IMAGE_CHAR);

    // Write the image to the given file.
    void write(const char* filename);
    
    void show(int r,int c,int w,int h);
    
    void emptyFrom(const Image& src);

    // Create an independent clone of the image
    void copyTo(Image& dst)const;

    // Assign the metadata to another image. The two image will share a same memory block.
    void assign(Image& dst)const;
    
    // Create an independent clone of the image
    Image clone(void)const;

    // Assign the metadata to another image. The two image will share a same memory block.
    Image operator=(const Image &dst){dst.assign(*this);return *this;}

    // ====================== Access Methods ======================

    //
    void* getPixelPtr(unsigned int r,unsigned int c)const;
    
    template <class T>
    T getPixel(unsigned int r,unsigned int c,unsigned int ch)const
    {
        return *(T*)((char*)getPixelPtr(r,c)+eLen*ch);
    }

    // ====================== Operator ======================
    
    Image operator+(int c)const;
    Image operator-(int c)const;
    Image operator*(int c)const;
    Image operator/(int c)const;
    
    Image operator+(float c)const;
    Image operator-(float c)const;
    Image operator*(float c)const;
    Image operator/(float c)const;
    
    // ====================== Display Method ======================



    // ====================== Variables ======================

    // ---------------------- Properties of an image -------------------------
    
    // Width and height of the image
    unsigned int width;
    unsigned int height;
    // Number of channels of the image.
    unsigned int channel;
    // Pixel type.
    int type;
    
    // ------------------------- Memory Management ---------------------------
    
    // Pointing to the memory block of the image data.
    // Data are stored row by row, with no byte alignment.
    char *data;
    
    // The counter denoting the number of objects pointing to current memory block.
    // See the comment on the release() method.
    unsigned int  *copycount;

    // Accessories.
    // Index increment to access the next pixel in the same row and same channel.
    unsigned int dx;
    // Index increment to access the next pixel in the same column and same channel.
    unsigned int dy;
    // Size of the image = w*h
    unsigned int size;
    // Number of elements in the image = w*h*ch
    unsigned int numel;
    // The memory size of data measured in bytes.
    unsigned int mSize;
    // element length.
    unsigned int eLen;
    // Same as dx,dy but measured by byte.
    unsigned int c_dx;
    unsigned int c_dy;

private:

    // Allocate memory for image data.
    void allocate(unsigned int length);
    
    // Detach the object from the memory block that contains image data.
    // if this object is the last one that points to the block, it deallocates
    // the memory block, otherwise it does nothing to the block and simply sets the
    // pointers as 0. This is realized by creating a integer counter maintained by all objects
    // pointing to a same block. The counter shows how many objects are pointing to the block.
    void release(void);

};

class ROIScanner
{
public:
    // 
    ROIScanner(const Image &im,int r,int c,int w,int h){setup(im,r,c,w,h);}
    ROIScanner(const Image &image){setup(image);}

    // Setup the scannner
    // r(row),c(column),w(width),h(height) define a rect region.
    void setup(const Image &image,int r,int c,int w,int h);
    // Scan the entire image.
    void setup(const Image &image);
    
    // Rest the state to just after setup() is called.
    void restart(void);
    
    // Go to the next pixel. (like i++)
    bool next(void);
    
    // Get the pointer of the current pixel.
    template <class T>
    T* ptr(void)const{return (T*)cptr;}
    template <class T>
    T* ptr(unsigned int i)const{return (T*)cptr+i;}
    
    // Write value to current pixel.
    void setPixel(const void* v){for(int i=0;i<c_dx;i++)cptr[i]=((char*)v)[i];}
    
    // Get the value of the current pixel.
    template <class T>
    T  value(void)const{return *(T*)cptr;}
    template <class T>
    T  value(int i)const{return *((T*)cptr+i);}
    
    //T  neighbor(int r,int c,int i=0){return *((T*)(cptr+r*dy+c*dx)+i);}

    // Get the row    of the current pixel.
    int currentRow(void)const{return regR+cntR;}
    
    // Get the column of the current pixel.
    int currentCol(void)const{return regC+cntC;}
    
    // Set if the scanning comes to end.
    bool isEnd(void)const{return !isReadyFlag;}
    
    // Set if current state is valid.
    bool isReady(void)const{return isReadyFlag;}
    
    // Set when the row changes in last turn.
    bool isRowChanged(void)const{return isRowChangeFlag;}
    
private:
    
    bool isSetupFlag;
    bool isReadyFlag;
    bool isRowChangeFlag;
    
    // Start position (use for restart).
    char *optr;
    // Current position
    char *cptr;
    /// The size and upper-left corner of the ROI.
    int regR,regC,regW,regH;
    // Row and column counters.
    int cntR,cntC;
    // c_dx,c_dy. Copied from the image.
    int c_dx,c_dy;
};





#endif





