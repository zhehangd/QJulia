#ifndef UTILITY_H_
#define UTILITY_H_

#include <iostream>
#include <chrono>
#include "Math.hpp"

std::ostream& operator<<(std::ostream& ss,const Vector3 &v);

class Timer
{
public:
  void start(void);
  float duration(void);
  std::chrono::steady_clock::time_point srt;
};

#endif