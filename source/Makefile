# File:  makefile
# Name:  Zhehang Ding
# Email: zhehangd@gmail.com
# Data:  07/08/2017


CXX      = g++
DEBUG    = -g
CXXFLAGS = -std=c++11 -O3 -Iinc
VPATH    = lib src inc


.PHONY: clean

ALL   : demo


demo  : Image.o Math.o Camera.o Console.o main.o
	$(CXX) -o $@ $^

main.o: Image.hpp Math.hpp Camera.hpp Console.hpp

clean:
	@echo clean
	rm *.o *~ demo
