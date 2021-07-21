cmake_minimum_required(VERSION 3.20)

project(GFX LANGUAGES CXX)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
	set(GFX_STANDALONE TRUE)
endif()

option(GFX_BUILD_EXAMPLES "Build the GFX example programs" ${GFX_STANDALONE})

# Specify C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(Dependencies.cmake)
add_subdirectory(GFX)

if(GFX_BUILD_EXAMPLES)
	add_subdirectory(Examples)
endif()