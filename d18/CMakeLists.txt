cmake_minimum_required(VERSION 3.30)
project(d18)

set(CMAKE_CXX_STANDARD 20)

add_executable(d18 src/main.cpp
        src/coordinates.cpp
        src/coordinates.h)
