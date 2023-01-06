pjlz: Makefile main.cpp include/int-types.hpp include/nearest-prefix.hpp include/slurp.hpp include/suffix-sort.hpp include/util.hpp
	g++ -I include/ -Wall -O -o pjlz main.cpp
