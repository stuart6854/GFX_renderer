cmake_minimum_required(VERSION 3.19)
project(Personal_Renderer)

set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED FATAL_ERROR)

add_subdirectory(dependencies)
add_subdirectory(Renderer)
add_subdirectory(examples)