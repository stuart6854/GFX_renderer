add_library(gfx
        include/GFX/Debug.h
        include/GFX/DeviceContext.h
        include/GFX/RenderContext.h
        include/GFX/Resources/Buffer.h
        include/GFX/Resources/Framebuffer.h
        include/GFX/Resources/Pipeline.h
        include/GFX/Resources/ResourceDescriptions.h
        include/GFX/Resources/Shader.h
        include/GFX/Resources/ShaderUniform.h
        include/GFX/Resources/VertexLayout.h
        include/GFX/Surface.h src/Platform/Vulkan/VulkanSurface.h
        include/GFX/Utility/Color.h
        src/GFX/CommandBuffer.h
        src/GFX/GFXAPI.h
        src/GFX/Renderers/RendererForward.cpp
        src/GFX/Resources/Mesh.cpp
        src/GFX/Resources/ShaderUniform.cpp
        src/GFX/Utility/Color.cpp
        src/Platform/Vulkan/VulkanAllocator.cpp
        src/Platform/Vulkan/VulkanAllocator.h
        src/Platform/Vulkan/VulkanBuffer.cpp
        src/Platform/Vulkan/VulkanBuffer.h
        src/Platform/Vulkan/VulkanCommandBuffer.cpp
        src/Platform/Vulkan/VulkanCommandBuffer.h
        src/Platform/Vulkan/VulkanCore.cpp
        src/Platform/Vulkan/VulkanCore.h
        src/Platform/Vulkan/VulkanDeviceContext.cpp
        src/Platform/Vulkan/VulkanDeviceContext.h
        src/Platform/Vulkan/VulkanFramebuffer.cpp
        src/Platform/Vulkan/VulkanFramebuffer.h
        src/Platform/Vulkan/VulkanGFX.cpp
        src/Platform/Vulkan/VulkanPipeline.cpp
        src/Platform/Vulkan/VulkanPipeline.h
        src/Platform/Vulkan/VulkanRenderContext.cpp
        src/Platform/Vulkan/VulkanRenderContext.h
        src/Platform/Vulkan/VulkanShader.cpp
        src/Platform/Vulkan/VulkanShader.h
        src/Platform/Vulkan/VulkanUtils.cpp
        src/Platform/Vulkan/VulkanUtils.h
        include/GFX/Resources/UniformBuffer.h src/Platform/Vulkan/VulkanUniformBuffer.cpp src/Platform/Vulkan/VulkanUniformBuffer.h include/GFX/Resources/UniformBufferSet.h src/GFX/Resources/UniformBufferSet.cpp include/GFX/Renderers/RendererStructures.h)

target_include_directories(gfx
    PUBLIC
        src
        include
        $ENV{VULKAN_SDK}/Include)

target_link_directories(gfx
    PUBLIC
        $ENV{VULKAN_SDK}/Lib
        ${CMAKE_SOURCE_DIR}/dependencies/VulkanSDK/Lib)

set(VulkanLibs
        optimized spirv-cross-core spirv-cross-cpp shaderc_shared
        debug spirv-cross-cored spirv-cross-cppd shaderc_sharedd)

target_link_libraries(gfx PUBLIC fmt::fmt glm vulkan-1 ${VulkanLibs} d3d12 assimp::assimp)

target_compile_definitions(gfx PUBLIC GFX_API_VULKAN)

