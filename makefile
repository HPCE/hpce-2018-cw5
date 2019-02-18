SHELL=/bin/bash

CPPFLAGS += -std=c++11 -W -Wall  -g
CPPFLAGS += -O3
CPPFLAGS += -I include

ifeq ($(OS),Windows_NT)
LDLIBS += -lws2_32 
else
LDLIBS += -lrt
endif

# These libraries will always be linked in when `libpuzzler.a` is compiled
# into programs used in assessment. If you don't need them, feel free to
# comment them out.
LDLIBS += -ltbb -lOpenCL

ifeq ($(findstring MINGW,$(shell uname)),MINGW)
LDLIBS := $(subst -lOpenCL,$(shell which OpenCL.dll),$(LDLIBS))
endif

all : bin/execute_puzzle bin/create_puzzle_input bin/run_puzzle bin/compare_puzzle_output

lib/libpuzzler.a : $(wildcard provider/*.cpp provider/*.hpp include/puzzler/*.hpp include/puzzler/*/*.hpp)
	cd provider && $(MAKE) all

bin/% : src/%.cpp lib/libpuzzler.a
	-mkdir -p bin
	$(CXX) $(CPPFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS) -Llib -lpuzzler
