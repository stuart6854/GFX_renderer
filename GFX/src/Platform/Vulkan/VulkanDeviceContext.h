//
// Created by stumi on 07/06/21.
//

#ifndef PERSONAL_RENDERER_VULKANDEVICECONTEXT_H
#define PERSONAL_RENDERER_VULKANDEVICECONTEXT_H

#include "VulkanSurface.h"
#include "VulkanAllocator.h"
#include "VulkanFramebuffer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanBuffer.h"
#include "VulkanShader.h"
#include "VulkanTexture.h"

#include "GFX/Config.h"
#include "GFX/Resources/ResourceDescriptions.h"

#include <vulkan/vulkan.hpp>

#include <memory>

namespace gfx
{
    class RenderContext;

    /**
     * Device Context is basically equal to a window.
     *
     * It manages window-side resources such as frame buffers, describes pixel formats of the window and frame buffer, etc.
     */
    class DeviceContext
    {
    public:
        DeviceContext();
        ~DeviceContext();

        void ProcessWindowChanges(ISurface& surface, uint32_t windowWidth, uint32_t windowHeight);

        auto CreateBuffer(BufferDesc desc) -> std::shared_ptr<Buffer>;
        auto CreateShader(const std::string& path) -> std::shared_ptr<Shader>;
        auto CreatePipeline(const PipelineDesc& desc) -> std::shared_ptr<Pipeline>;

        void Upload(Buffer* dst, const void* data);
        void Upload(Texture* texture);

        void NewFrame();

        void Submit(RenderContext& context);
        void Submit(CommandBuffer& cmdBuffer);
        void Present();

        auto GetCurrentFrameIndex() const -> uint32_t { return m_frameCounter % Config::FramesInFlight; }
        auto GetFramebuffer() -> std::shared_ptr<Framebuffer>;

    private:
        struct Frame
        {
            vk::Semaphore PresentComplete, RenderComplete;
            vk::Fence RenderFence;
        };

        auto GetCurrentFrame() -> Frame&;

        void CreateSwapchain(uint32_t width, uint32_t height);

        void CreateDepthStencil();

        void CreateRenderPass();
        void CreateImageResources();
        void CreateFramesResources();

        void DestroySwapchainResources();
        void Destroy();

    private:
        vk::SurfaceKHR m_surface;

        vk::SwapchainKHR m_swapchain;
        uint32_t m_swapchainWidth;
        uint32_t m_swapchainHeight;
        vk::Format m_swapchainFormat;
        uint32_t m_swapchainImageIndex;
        std::vector<vk::Image> m_swapchainImages;
        std::vector<vk::ImageView> m_swapchainImageViews;
        std::vector<vk::Framebuffer> m_swapchainFramebuffers;

        struct
        {
            vk::Image Image;
            VmaAllocation Allocation;
            vk::ImageView View;
        } m_depthStencil;

        vk::RenderPass m_swapchainRenderPass;

        std::array<Frame, Config::FramesInFlight> m_frames;
        uint32_t m_frameCounter;

        /* Resources */
        std::vector<std::shared_ptr<Buffer>> m_buffers;
        std::vector<std::shared_ptr<Shader>> m_shaders;
        std::vector<std::shared_ptr<Pipeline>> m_pipelines;
    };
}  // namespace gfx

#endif  // PERSONAL_RENDERER_VULKANDEVICECONTEXT_H
