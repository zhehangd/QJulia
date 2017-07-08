# QJulia

## Log

### v0.2 In Progress

Adding multithreading support.

---

### v0.1 Feb.11,2016






Hello, QJulia v0.1 is released. This program generates Quaternion Julia Fractal images. The implementation mostly refers to
> Rosa, Alessandro. "Methods and applications to display quaternion Julia sets." Electronic Journal of Differential Equations and Control Processes, St. Petersburg 4 (2005): 1-22.

Here is a brief explaination about how this program works. The key idea is, we divide the space into a dense 3D grid, take one sample from each cell and see whether it belongs to the fractal set (iteration convergent). We choose the coordinate system of our camera to make this division. So a stack of cells along the depth axis is aligned with a pixel. We then search along the stack one by one until we find a fractal point. We refine our finding by searching between the two cells we just computed, and locate the surface of our fractal(there is no such a real surface for a fractal, it is more like a contour line). A surface map is then generate when we finished this searching for all pixels. In the surface map each pixel has three channels which store the x,y,z coordinate of the surface point it detected. This procedure is the most time consuming part of our program. Next, we try to estimate the normal vectors of the surface from our surface map. This is done by take difference of adjcent pixels. And use cross product to get the normals. For an on-surface point this gives a good estimation of the normal. For an on-edge point the normal mostly is parallel to the screen, because the adjacent pixel it takes is on the surface behind of in front of it, causing a wrong estimation. However, this surprisingly gives a very good appearance of an edge. The outcome of this step is a normal map, where each pixel cotains a normal vector. This the last step, we use the Phong model to assign colors to the fractal, based on the configuration of lights.

Current Features:
 * Perspective Projection.
 * Lighting Effect.
 * Basic Camera Control.


## Usage

Usage: `./QJulia [-option1 value] [-option2 value] ...`


| option    | name             | example             |
| --------- |:----------------:| -------------------:|
|  -s       | size             | "800 600"
|  -f       | magnification    | 1.0
|  -d       | division         | 200
|  -t       | threshold        | 20
|  -p       | precision        | 0.0001
|  -z       | depth            | "1 8"
|  -q       | constant         | "-0.2 0.8 0.0 0.0"
|  -v       | camera           | "60 30 4"
|  -C       | camera           | "4 3 2 0 0 0"
|  -l       | light            | SEE DESCRIPTION
|  -o       | filename         | Julia.data

*Size*: size of the image, expressed in "width height" format. The spatial resolution is the most important parameter to control the quality of the rendering, since it determines how accurate we estimate the normal vector. Personal opinion is, 512x512 gives a good preview, 1024x1024 gives an acceptable image, more then 3000x3000 gives a very beature result but also costs a lot of time.

*Magnification*: magnification (mag.) is a scalar that zooms in or out your view. Magnification will not change the shape and projection you get, which is not like changing distance we change the perspective effect.

*Division*: division controls how many cells you divide along the depth axis. The exact value should choose depends on how diverse your fractal is. You make use trail-and-error method to get the best configuration. If you set it too small, the fractal looks broken into pieces. If you set it too large it will waste a lot of time.

*Threshold*: how many iterations should be taken to decide whether a point is convergent. The larger the value is, the rich details you get. However, you must also increase division and resolution as well, or the sampling theorem will make the image very ugly.

*Precision*: how accurate it should be when doing the locaton refinement. Usually you do not need to change it. And be careful do not let this value exceeds the low bound of the float number precision, or it will never converge.

*Depth*: this determines the searching depth, consists of two value, zmin and zmax. A good pair of values should be chosen to maximize the searching efficiency.

*Quaternion Constant*: this is the constant that defines which Julia fractal you want. See the definition of the quaternion Julia set.

*Camera*: there are two way to set up your camera. First is denoting the polor angles and the radius, the camera will points at the origin. Or you can denotes the Euclidean coordinate of where the camera is and where it points to. In this first way, you use -c option, follow three values: Horizontal Angle, Vertical Angle, and Radius. The two angleis is given in degree. Use -C to give Euclidean coordinates. The first three are your camera location in x,y,z, the second three are the point you look at. Note that y-axis is the upward and it is a right-hand coordinate system. 

*Lighting*: there are lots of parameters you can define the lighting environment. This system is not yet fully complete, but it is workable. A light control argument begins by an option -l, followed by a number chosen from 0,1,2 which indicates the light you are setting, then a character chosen from 'a','d','p', which mean ambient, diffuse, and position. At last you should give three numbers to assign to the property you choose. A example is -l "0 p 0.5 0.3 0.0" which set the light0 to position (0.5,0.3,0).

*Filename*: this is the file name you want for your output. Only raw data is supported, changing extension will not change this fact.
