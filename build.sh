#!/bin/sh 
#arm-linux-gnueabihf-g++-8 gamecode.cpp -fno-access-control -fno-exceptions -fno-rtti -std=c++11 -lpthread -lm -o gamecode.o
#arm-linux-gnueabihf-ld gamecode.o protowork.o -o protowork.o -std=c++11 -lpthread -lm -lSDL2 
arm-linux-gnueabihf-g++ main.cpp -fno-access-control -fno-exceptions -fno-rtti -std=c++11 snesclassic/libSDL2.so snesclassic/libm-2.23.so -o protowork

#-nostdlibs  -nodefaultlibs