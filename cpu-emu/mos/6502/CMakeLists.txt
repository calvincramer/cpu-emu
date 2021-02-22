cmake_minimum_required(VERSION 3.16.3)

# set the project name
project(cpu-emu)

# cpp standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
 
# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
 
add_subdirectory (src) 
add_subdirectory (test)
