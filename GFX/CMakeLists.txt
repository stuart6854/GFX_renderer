
message(STATUS "GFX Graphics API=${GFX_GRAPHICS_API}")

add_library(gfx STATIC)

add_subdirectory(src)

target_include_directories(gfx PUBLIC include src)

target_link_libraries(gfx PUBLIC Vulkan::Vulkan glfw glm fmt stb assimp shaderc spirv-cross-core spirv-cross-glsl)

target_compile_definitions(gfx 
    PUBLIC 
        GLFW_INCLUDE_VULKAN 
        GLM_FORCE_DEPTH_ZERO_TO_ONE
        GLM_FORCE_LEFT_HANDED 
        ${GFX_GRAPHICS_API}
    )