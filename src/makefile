# File:  makefile
# Name:  Zhehang Ding
# Email: dingzhehang1993@gmail.com
# Data:  Feb. 08, 2016


CPP       = g++
DEBUG    = -g
CXXFLAGS =  -std=c++11 -O3
# LXXFLAGS =  $(DEBUG)

ALL   : QJulia

QJulia  : Image.o Math.o main.o Parser.o QuatJulia.o
	$(CPP) -o QJulia  Image.o Math.o main.o Parser.o QuatJulia.o

main.o :   Image.h Math.h Parser.h main.cpp QuatJulia.h
QuatJulia.o: Image.h Math.h QuatJulia.cpp



clean:
	rm *.o *~ QJulia
