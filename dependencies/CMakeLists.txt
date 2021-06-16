set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
add_subdirectory(glfw)

add_subdirectory(glm)

add_subdirectory(SPIRV-Tools)

add_subdirectory(glslang)

set(SHADERC_SKIP_INSTALL ON)
set(SHADERC_SKIP_TESTS ON)
set(SHADERC_SKIP_EXAMPLES ON)
set(SKIP_GLSLANG_INSTALL ON)
set(SKIP_SPIRV_TOOLS_INSTALL ON)
set(SKIP_GOOGLETEST_INSTALL ON)
add_subdirectory(shaderc)

option(SPIRV_CROSS_ENABLE_TESTS "Enable SPIRV-Cross tests." OFF)
option(SPIRV_CROSS_ENABLE_HLSL "Enable HLSL target support." OFF)
option(SPIRV_CROSS_SKIP_INSTALL "Skips installation targets." ON)
option(SPIRV_CROSS_ENABLE_REFLECT "Enable JSON reflection target support." OFF)
add_subdirectory(spirv-cross)