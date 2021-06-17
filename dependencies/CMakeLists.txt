add_subdirectory(fmt)

set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
add_subdirectory(glfw)

add_subdirectory(glm)

option(SPIRV_SKIP_EXECUTABLES "Skip building the executable and tests along with the library" ON)
add_subdirectory(SPIRV-Tools)

option(SKIP_GLSLANG_INSTALL "Skip installation" ON)
option(ENABLE_CTEST "Enables testing" OFF)
add_subdirectory(glslang)

set(SHADERC_SKIP_INSTALL ON)
set(SHADERC_SKIP_TESTS ON)
set(SHADERC_SKIP_EXAMPLES ON)
set(SKIP_GLSLANG_INSTALL ON)
set(SKIP_SPIRV_TOOLS_INSTALL ON)
set(SKIP_GOOGLETEST_INSTALL ON)
add_subdirectory(shaderc)

option(SPIRV_CROSS_ENABLE_TESTS "Enable SPIRV-Cross tests." OFF)
option(SPIRV_CROSS_SKIP_INSTALL "Skips installation targets." ON)
add_subdirectory(spirv-cross)