#!/bin/make
CXX = g++
CXXFLAGS = -O3 -Wall

OPENCVINC = `pkg-config --cflags opencv`
OPENCVLIB = `pkg-config --libs opencv`

LDFLAGS  = -fopenmp

OBJS = main

.PHONY: all clean
.SUFFIXES: .c .cpp .o

all:
	$(CXX) $(CXXFLAGS) -o color.o -c color.cpp
	$(CXX) $(CXXFLAGS) -o vector3.o -c vector3.cpp
	$(CXX) $(CXXFLAGS) -o polygon3.o -c polygon3.cpp
	$(CXX) $(CXXFLAGS) -o aabb3.o -c aabb3.cpp
	$(CXX) $(CXXFLAGS) -o screen.o -c screen.cpp $(OPENCVINC)
	$(CXX) $(CXXFLAGS) -o scene.o -c scene.cpp
	$(CXX) $(CXXFLAGS) -o main.o -c main.cpp $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -o main main.o color.o vector3.o polygon3.o aabb3.o screen.o scene.o $(OPENCVLIB) $(LDFLAGS)
	rm -rf *.o

clean:
	rm -rf *.o
	rm -rf main
