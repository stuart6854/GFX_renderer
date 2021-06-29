cmake_minimum_required(VERSION 3.20)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(GFX)

set(GFX_GRAPHICS_API "GFX_API_NONE" CACHE STRING "GFX graphics api")

include(Dependencies.cmake)
add_subdirectory(GFX)
add_subdirectory(Examples)