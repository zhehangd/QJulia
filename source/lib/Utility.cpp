#include "Utility.hpp"

#include <chrono>
#include <iostream>
#include <iomanip>

void Timer::start(void)
{
  srt=std::chrono::steady_clock::now();
}

float Timer::duration(void)
{
  return std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - srt).count();
}