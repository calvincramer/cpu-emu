cmake_minimum_required(VERSION 3.16.3)

# set the project name
project(cpu-emu)

# cpp standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Common build flags
set(CMAKE_CXX_FLAGS "-Werror -Wall -Wpedantic -Wextra -fexceptions")

# Debug flags
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Other nodes to build
add_subdirectory (src)
add_subdirectory (test)
