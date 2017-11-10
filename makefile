SHELL=/bin/bash

CPPFLAGS += -std=c++11 -W -Wall  -g
CPPFLAGS += -O3
CPPFLAGS += -I include

ifeq ($(OS),Windows_NT)
LDLIBS += -lws2_32
else
LDLIBS += -lrt
endif

all : bin/execute_puzzle bin/create_puzzle_input bin/run_puzzle bin/compare_puzzle_output

lib/libpuzzler.a : $(wildcard provider/*.cpp provider/*.hpp include/puzzler/*.hpp include/puzzler/*/*.hpp)
	cd provider && $(MAKE) all

bin/% : src/%.cpp lib/libpuzzler.a
	-mkdir -p bin
	$(CXX) $(CPPFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS) -Llib -lpuzzler


serenity_now_% : all
	mkdir -p w
	bin/run_puzzle $* 100 2
	bin/create_puzzle_input $* 100 2 > w/$*.in
	cat w/$*.in | bin/execute_puzzle 1 2 > w/$*.ref.out
	cat w/$*.in | bin/execute_puzzle 0 2 > w/$*.got.out
	bin/compare_puzzle_output w/$*.ref.out w/$*.got.out 2

serenity_now : $(foreach x,julia ising_spin logic_sim random_walk,serenity_now_$(x))


##############################################
## Deal with floating-point checking for julia

# Create reference version on an IEEE compliant CPU+compiler
#ref/%.ref.gz : ref/%.in.gz all
#	mkdir -p w
#	gunzip -d -c ref/$*.in.gz | bin/execute_puzzle 1 1 | gzip -c - > ref/$*.ref.gz

# Decompress reference input into working directory
w/%.in : ref/%.in.gz
	mkdir -p w
	gunzip -c ref/$*.in.gz > w/$*.in

# Expand the golden version
w/%.ref.global : ref/%.ref.gz
	mkdir -p w
	gunzip -c ref/$*.ref.gz > w/$*.ref.global

# Run current reference on reference input
w/%.ref.local : w/%.in w/%.ref.global all
	mkdir -p w
	cat w/$*.in | bin/execute_puzzle 1 2 > w/$*.ref.local
	bin/compare_puzzle_output w/$*.ref.local w/$*.ref.global 3

# Run current version on reference input ($<) and check it
# Add dependency on all so that it will re-run if
# the program changes
w/%.got : w/%.in  w/%.ref.global  all
	cat w/$*.in | bin/execute_puzzle 0 2 > w/$*.got
	bin/compare_puzzle_output w/$*.got w/$*.ref.global 3


check_julia_ref_local : $(patsubst ref/julia.%.in.gz,w/julia.%.ref.local,$(wildcard ref/julia.*.in.gz))

check_julia_local : $(patsubst ref/julia.%.in.gz,w/julia.%.got,$(wildcard ref/julia.*.in.gz))

check_julia : check_julia_ref_local check_julia_local

# Check platforms:
#   g++5.4, CYGWIN_NT-10.0-WOW GABBY 2.6.0(0.304/5/3) 2016-08-31 14:27 i686 Cygwin
#	g++6.2, MINGW64_NT-10.0 GABBY 2.6.0(0.304/5/3) 2016-09-07 20:45 x86_64 Msys
#   g++5.4, Linux contrib-jessie 3.16.0-4-amd64 #1 SMP Debian 3.16.36-1+deb8u1 (2016-09-03) x86_64 GNU/Linux
#	(AWS CPU) g++5.4, Linux vm-shell1.doc.ic.ac.uk 4.4.0-47-generic #68-Ubuntu SMP Wed Oct 26 19:39:52 UTC 2016 x86_64 x86_64 x86_64 GNU/Linux
#	(AWS GPU) g++5.4, Linux vm-shell1.doc.ic.ac.uk 4.4.0-47-generic #68-Ubuntu SMP Wed Oct 26 19:39:52 UTC 2016 x86_64 x86_64 x86_64 GNU/Linux
