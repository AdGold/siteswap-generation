CXXFLAGS += -O3 --std=c++2a

.PHONY : all

all: siteswapping antipermutation stack count_prime gen_comp

clean:
	rm siteswapping antipermutation stack count_prime gen_comp

siteswapping: siteswapping.cpp
antipermutation: antipermutation.cpp
stack: stack.cpp
count_prime: count_prime.cpp
gen_comp: gen_comp.cpp

