cmake_minimum_required(VERSION 3.30)
project(d17)

set(CMAKE_CXX_STANDARD 20)

add_executable(d17 ./src/main.cpp
        src/uint3_t.cpp
        src/uint3_t.h)
