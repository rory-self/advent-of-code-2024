cmake_minimum_required(VERSION 3.30)
project(advent_of_code_2024)

set(CMAKE_CXX_STANDARD 23)

set(COMPLETE_DAYS 01 02 03 04 05 07 08 09 10 11 12 13 15 19)
foreach(DAY ${COMPLETE_DAYS})
  add_executable(d${DAY} src/d${DAY}/main.cpp)
endforeach()

# Additional files
add_executable(d06 src/d06/main.cpp src/d06/guard.cpp src/d06/guard.h)
add_executable(d14 src/d14/main.cpp src/d14/robot.cpp src/d14/robot.h src/d14/room.h src/d14/room.cpp)
add_executable(d16 src/d16/main.cpp src/d16/coordinates.cpp src/d16/coordinates.h)
add_executable(d17 src/d17/main.cpp src/d17/uint3_t.cpp src/d17/uint3_t.h)
add_executable(d18 src/d18/main.cpp src/d18/coordinates.cpp src/d18/coordinates.h)

