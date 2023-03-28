cmake_minimum_required(VERSION 3.23)
project(parallelProgramming)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fopenmp -msse")

set(SOURCE_FILES main.cpp)
add_executable(openmp_test_clion ${SOURCE_FILES})
add_executable(parallelProgramming main.cpp)
