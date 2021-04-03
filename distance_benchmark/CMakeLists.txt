# CMakeList.txt : CMake project for distance_benchmark, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (distance_benchmark "distance_benchmark.cpp" "distance_benchmark.h" "nanobench.h")

# TODO: Add tests and install targets if needed.
