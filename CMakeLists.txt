cmake_minimum_required(VERSION 3.19)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/dependencies/vcpkg/scripts/buildsystems/vcpkg.cmake
        CACHE STRING "Vcpkg toolchain file")

project(Personal_Renderer)

set(CMAKE_CXX_STANDARD 20)

find_package(assimp CONFIG REQUIRED)

add_subdirectory(dependencies)
add_subdirectory(Renderer)
add_subdirectory(examples)