cmake_minimum_required(VERSION 3.15)
project(test)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(simple-geometry REQUIRED)

add_executable(test test.cpp)
target_link_libraries(test simple-geometry::simple-geometry)
