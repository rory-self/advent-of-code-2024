cmake_minimum_required(VERSION 3.30)
project(d16)

set(CMAKE_CXX_STANDARD 20)

add_executable(d16 src/main.cpp
        src/Coordinates.cpp
        src/Coordinates.h
        src/Reindeer.cpp
        src/Reindeer.h)
