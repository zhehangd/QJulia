// File:   Perlin3d.h
// Author: Zhehang Ding
// Email:  dingzhehang1993@gmail.com
// Data:   Oct. 02, 2016

#ifndef PERLIN3D_H_
#define PERLIN3D_H_
#include <iostream>
#include <cstdint>

// 3D Perlin Noise
// This class generates 3D Perlin noise.
// The seed can be denoted by [setseed], which accepts any int number.
// Use [nextseed] to generate a new seed value based on the current seed.
// Same seed always generates same noise value at same position.
// Same seed always generates same result in [setseed].
// [perlin] generates a noise value given [x,y,z,stride].
// [x,y,z] is the position of the noise value to be generated.
// [stride] denotes the noise frequency.
// The randomness of the noise depends only on [seed] and stride-normalized [x,y,z].
// Author: Zhehang Ding | Date: Oct.01 2016 | License: BSD | Standard: C++11
class Perlin3D
{
public:
  // Constructor
  Perlin3D(void){setseed(0);}
  ~Perlin3D(void){}
  
  // Set the seed for the generator.
  void setseed(int x);
  
  // Generate a new pseudorandom seed from the current one.
  void nextseed(void){seed *= 3346547543;}
  
  // Query the noise value given position and frequency.
  float perlin(float x,float y,float z,float stride)const;

private:
  
  // Smooth interpolation function.
  float interpolate(float a,float b,float x)const;
  
  // Query the gradiant at a integer position.
  void getGradient(int xyz[3],float g[3])const;
  
  // Seed.
  std::uint32_t seed;
  
};

#endif