# File:  makefile
# Name:  Zhehang Ding
# Email: zhehangd@usc.edu
# Data:  Feb. 08, 2016


CPP       = g++
DEBUG    = -g
CXXFLAGS =  -std=c++11 -O3 $(DEBUG)
LXXFLAGS =  $(DEBUG)

ALL   : QJulia

QJulia  : Image.o Math.o main.o QuatJulia.o
	$(CPP) -o QJulia  Image.o Math.o main.o QuatJulia.o

main.o :   Image.h Math.h main.cpp QuatJulia.o
QuatJulia.o: Image.h Math.h QuatJulia.cpp

clean:
	rm *.o *~ QJulia
