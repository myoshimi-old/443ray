#!/bin/make

TARGET = ./main$(EXEEXT)
SRCS = main.cpp color.cpp vector3.cpp polygon3.cpp aabb3.cpp light.cpp screen.cpp scene.cpp
CXX = g++
CXXFLAGS = -O3 -Wall -g

OPENCVINC = `pkg-config --cflags opencv`
OPENCVLIB = `pkg-config --libs opencv`

LDFLAGS  = -fopenmp

OBJS = $(SRCS:.cpp=.o)

.PHONY: all clean
.SUFFIXES: .cpp .o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS) $(OPENCVLIB)

.cpp.o:
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OPENCVINC) -c $<

depend:
	$(CXX) -MM $(INCLUDE) $(CXXFLAGS) $(SRCS) > dependencies

clean:
	rm -rf $(OBJS) $(TARGET)


