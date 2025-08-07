cmake_minimum_required(VERSION 3.1)
project(test-render-sdlgl-example VERSION 0.1.0)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -ggdb")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME} test.cpp)

include(FetchContent)
# https://gist.github.com/Lightnet/398d2ff0b8691aa03520b45b1b4ef69c
# FetchContent_Declare(SDL2
#GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
#GIT_TAG release-2.26.3
#GIT_SHALLOW TRUE
#GIT_PROGRESS FALSE
# )
# FetchContent_MakeAvailable(SDL2)

find_package(SDL2 REQUIRED)

# https://jonathanhamberg.com/post/2019-01-27-cmake-dependencies-fetch-content/
FetchContent_Declare(glad
  GIT_REPOSITORY https://github.com/Dav1dde/glad.git
  GIT_SHALLOW TRUE
)

FetchContent_GetProperties(glad)
if(NOT glad_POPULATED)
    FetchContent_Populate(glad)
    set(GLAD_PROFILE "core" )
	# API type/version pairs, like \"gl=3.2,gles=\", no version means latest
    set(GLAD_API "gl=3.2")
	# Language to generate the binding for
    set(GLAD_GENERATOR "c" )
    add_subdirectory(${glad_SOURCE_DIR} ${glad_BINARY_DIR})
endif()

# https://gamedev.stackexchange.com/questions/208910/cmakelists-txt-with-sfml-glew-glm-exe-doesnt-work
# https://github.com/QuentinManemla/Bomberman/blob/master/CMakeLists.txt
FetchContent_Declare(glm
  GIT_REPOSITORY	https://github.com/g-truc/glm.git
  GIT_TAG 	bf71a834948186f4097caa076cd2663c69a10e1e #refs/tags/1.0.1
  GIT_SHALLOW TRUE
  GIT_PROGRESS FALSE
)
set(GLM_TEST_ENABLE OFF)
FetchContent_MakeAvailable(glm)

target_link_libraries(${PROJECT_NAME} PUBLIC 
  glad
  SDL2::SDL2-static
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
	${glm_INCLUDE_DIRS}
	${glm_SOURCE_DIR}
)
