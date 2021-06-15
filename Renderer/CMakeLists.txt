add_library(gfx
        include/GFX/DeviceContext.h
        src/GFX/GFXAPI.h
        include/GFX/RenderContext.h
        src/GFX/Renderers/RendererForward.cpp
        src/Platform/Vulkan/VulkanGFX.cpp
        src/Platform/Vulkan/VulkanDeviceContext.cpp
        src/Platform/Vulkan/VulkanDeviceContext.h
        src/Platform/Vulkan/VulkanRenderContext.cpp
        src/Platform/Vulkan/VulkanRenderContext.h
        src/Platform/Vulkan/VulkanCore.cpp
        src/Platform/Vulkan/VulkanCore.h
        src/Platform/Vulkan/VulkanUtils.cpp
        src/Platform/Vulkan/VulkanUtils.h
        src/GFX/CommandBuffer.h
        src/Platform/Vulkan/VulkanCommandBuffer.cpp
        src/Platform/Vulkan/VulkanCommandBuffer.h
        include/GFX/Surface.h src/Platform/Vulkan/VulkanSurface.h
        include/GFX/Utility/Color.h
        src/GFX/Utility/Color.cpp
        include/GFX/Resources/Framebuffer.h
        src/Platform/Vulkan/VulkanFramebuffer.cpp
        src/Platform/Vulkan/VulkanFramebuffer.h
        src/Platform/Vulkan/VulkanAllocator.cpp
        src/Platform/Vulkan/VulkanAllocator.h
        include/GFX/Resources/Buffer.h
        src/Platform/Vulkan/VulkanBuffer.cpp
        src/Platform/Vulkan/VulkanBuffer.h
        include/GFX/Resources/ResourceDescriptions.h
        src/Platform/Vulkan/VulkanShader.cpp
        src/Platform/Vulkan/VulkanShader.h
        include/GFX/Resources/Shader.h
        src/Platform/Vulkan/VulkanPipeline.cpp
        src/Platform/Vulkan/VulkanPipeline.h include/GFX/Resources/VertexLayout.h include/GFX/Resources/Pipeline.h)

target_include_directories(gfx PUBLIC src include)

target_link_libraries(gfx PUBLIC Vulkan::Vulkan d3d12.lib shaderc)

target_compile_definitions(gfx PUBLIC GFX_API_VULKAN)

