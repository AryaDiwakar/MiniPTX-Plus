# MiniPTX Makefile
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -Wpedantic -O2 -Iinclude
LDFLAGS =

SRCS = $(wildcard src/**/*.cpp src/*.cpp)
OBJS = $(SRCS:.cpp=.o)

# flatten
SRCFILES = $(shell find src -name '*.cpp')
OBJFILES = $(SRCFILES:.cpp=.o)

all: miniptx

miniptx: $(OBJFILES)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJFILES) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJFILES) miniptx

.PHONY: all clean
