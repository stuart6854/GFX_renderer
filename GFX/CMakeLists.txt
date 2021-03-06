set(GFX_HEADERS 
    "include/GFX/GFX.h"
    "include/GFX/Config.h"
    "include/GFX/Debug.h"
    "include/GFX/Core/GFXCore.h"
    "include/GFX/Core/Base.h"
    "include/GFX/Core/Backend.h"
    "include/GFX/Core/Window.h"
    "include/GFX/Core/SwapChain.h"
    "include/GFX/Rendering/Renderer.h"
    "include/GFX/Resources/Framebuffer.h"
    "include/GFX/Resources/CommandBuffer.h"
    "include/GFX/Resources/Vertex.h"
    "include/GFX/Resources/Buffer.h"
    "include/GFX/Resources/Mesh.h"
    "include/GFX/Resources/Shader.h"
    "include/GFX/Resources/VertexLayout.h"
    "include/GFX/Resources/Pipeline.h"
    "include/GFX/Resources/Viewport.h"
    "include/GFX/Resources/Scissor.h"
    "include/GFX/Resources/ResourceSetLayout.h"
    "include/GFX/Resources/ResourceSet.h"
    "include/GFX/Resources/UniformBuffer.h"
    "include/GFX/Resources/MeshBuilder.h"
    "include/GFX/Resources/MeshImporter.h"
    "include/GFX/Resources/TextureBuilder.h"
    "include/GFX/Resources/TextureImporter.h"
    "include/GFX/Resources/Texture.h"
    "include/GFX/Resources/Font.h"
	"include/GFX/Utility/RectPacker.h"
	"include/GFX/Utility/IO.h"
)

set(GFX_SOURCES
    "src/Debug.cpp"
    "src/Core/GFXCore.cpp"
	"src/Core/Backend.cpp"
	"src/Core/Window.cpp"
	"src/Core/SwapChain.cpp"
	"src/Resources/Framebuffer.cpp"
	"src/Resources/CommandBuffer.cpp"
	"src/Resources/Buffer.cpp"
	"src/Resources/Shader.cpp"
	"src/Resources/Pipeline.cpp"
	"src/Resources/ResourceSetLayout.cpp"
	"src/Resources/ResourceSet.cpp"
	"src/Resources/UniformBuffer.cpp"
	"src/Resources/TextureImporter.cpp"
	"src/Resources/TextureBuilder.cpp"
	"src/Resources/Texture.cpp"
	"src/Resources/Font.cpp"
	"src/Resources/MeshBuilder.cpp"
	"src/Resources/MeshImporter.cpp"
	"src/Utility/RectPacker.cpp"
	"src/Utility/IO.cpp"
	"src/Utility/Timer.h"
	"src/Utility/Timer.cpp"
	"src/Platform/Vulkan/vk_mem_alloc.h"
	"src/Platform/Vulkan/VulkanBackend.h"
	"src/Platform/Vulkan/VulkanBackend.cpp"
	"src/Platform/Vulkan/VulkanDevice.h"
	"src/Platform/Vulkan/VulkanDevice.cpp"
	"src/Platform/Vulkan/VulkanPhysicalDevice.h"
	"src/Platform/Vulkan/VulkanPhysicalDevice.cpp"
	"src/Platform/Vulkan/VulkanSwapChain.h"
	"src/Platform/Vulkan/VulkanSwapChain.cpp"
	"src/Platform/Vulkan/VulkanAllocator.h"
	"src/Platform/Vulkan/VulkanAllocator.cpp"
	"src/Platform/Vulkan/VulkanUtils.h"
	"src/Platform/Vulkan/VulkanUtils.cpp"
	"src/Platform/Vulkan/VulkanFramebuffer.h"
	"src/Platform/Vulkan/VulkanFramebuffer.cpp"
	"src/Platform/Vulkan/VulkanCommandBuffer.h"
	"src/Platform/Vulkan/VulkanCommandBuffer.cpp"
	"src/Platform/Vulkan/VulkanBuffer.h"
	"src/Platform/Vulkan/VulkanBuffer.cpp"
	"src/Platform/Vulkan/VulkanShader.h"
	"src/Platform/Vulkan/VulkanShader.cpp"
	"src/Platform/Vulkan/VulkanPipeline.h"
	"src/Platform/Vulkan/VulkanPipeline.cpp"
	"src/Platform/Vulkan/VulkanResourceSetLayout.h"
	"src/Platform/Vulkan/VulkanResourceSetLayout.cpp"
	"src/Platform/Vulkan/VulkanResourceSet.h"
	"src/Platform/Vulkan/VulkanResourceSet.cpp"
	"src/Platform/Vulkan/VulkanTexture.h"
	"src/Platform/Vulkan/VulkanTexture.cpp"
)

add_library(gfx ${GFX_HEADERS} ${GFX_SOURCES})

target_include_directories(gfx PUBLIC "include" PRIVATE "src")

target_link_libraries(gfx PUBLIC Vulkan::Vulkan glfw glm stb assimp shaderc spirv-cross-core spirv-cross-glsl freetype fmt::fmt)

target_compile_definitions(gfx 
    PUBLIC 
        GLFW_INCLUDE_VULKAN 
        GLM_FORCE_DEPTH_ZERO_TO_ONE
        GLM_FORCE_LEFT_HANDED 
        ${GFX_GRAPHICS_API}
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${GFX_HEADERS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source Files" FILES ${GFX_SOURCES})

target_compile_options(gfx PRIVATE "${CMAKE_CXX_FLAGS} /W4 /WX /EHsc")