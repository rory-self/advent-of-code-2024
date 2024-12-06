cmake_minimum_required(VERSION 3.30)
project(d06)

set(CMAKE_CXX_STANDARD 20)

add_executable(d06 src/main.cpp
        src/Guard.cpp
        src/Guard.h
)
