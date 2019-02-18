SHELL=/bin/bash

CPPFLAGS += -std=c++11 -W -Wall  -g
CPPFLAGS += -O3 -DNDEBUG=1
CPPFLAGS += -I ../include
CPPFLAGS += -Wno-unused-parameter -Wno-unused-variable

# Useful for profiling
CPPFLAGS += -fno-omit-frame-pointer

puzzles.o : $(wildcard *.hpp) $(wildcard ../include/puzzler/*.hpp ../include/puzzler/*/*.hpp)

../lib/libpuzzler.a : puzzles.o
	-mkdir -p ../lib
	ar cr ../lib/libpuzzler.a puzzles.o

all : ../lib/libpuzzler.a
