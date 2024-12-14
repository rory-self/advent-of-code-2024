cmake_minimum_required(VERSION 3.30)
project(d14)

set(CMAKE_CXX_STANDARD 20)

add_executable(d14 src/main.cpp
        src/robot.cpp
        src/robot.h
        src/room.h
        src/room.cpp)
