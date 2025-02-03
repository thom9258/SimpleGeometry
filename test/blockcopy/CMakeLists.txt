cmake_minimum_required(VERSION 3.1)
project(simple_geometry VERSION 0.1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -ggdb")

add_executable(${PROJECT_NAME} test.c)
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 99)

find_library(MATH_LIBRARY m)
if(MATH_LIBRARY)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${MATH_LIBRARY})
endif()

