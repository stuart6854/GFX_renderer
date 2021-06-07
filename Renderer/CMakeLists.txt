project(GFX)

find_package(Vulkan REQUIRED FATAL_ERROR)

add_library(GFX
        src/GFX/Renderers/ForwardRenderer.cpp
        src/Platform/Vulkan/gfx_vulkan.cpp
        src/GFX/GFXAPI.h)

target_include_directories(GFX PUBLIC include)

target_link_libraries(GFX PUBLIC Vulkan::Vulkan d3d12.lib)

target_compile_definitions(GFX PUBLIC GFX_API_VULKAN)

