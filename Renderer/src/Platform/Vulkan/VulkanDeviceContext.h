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

#include "GFX/Resources/ResourceDescriptions.h"

#include <vulkan/vulkan.hpp>

namespace gfx
{
    class RenderContext;

    constexpr int FRAME_OVERLAP = 2;

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

        auto CreateBuffer(BufferDesc desc) -> Buffer;

        void Upload(Buffer& dst, const void* data);

        void NewFrame();

        void Submit(RenderContext& context);
        void Submit(CommandBuffer& cmdBuffer);
        void Present();

        auto GetFramebuffer() -> Framebuffer;

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

        struct Frame
        {
            vk::Semaphore PresentComplete, RenderComplete;
            vk::Fence RenderFence;
        };
        std::array<Frame, FRAME_OVERLAP> m_frames;
        uint32_t m_frameCounter;

        auto GetCurrentFrame() -> Frame&;

        void CreateSwapchain(uint32_t width, uint32_t height);

        void CreateDepthStencil();

        void CreateRenderPass();
        void CreateImageResources();
        void CreateFramesResources();

        void DestroySwapchainResources();
        void Destroy();
    };
}  // namespace gfx

#endif  // PERSONAL_RENDERER_VULKANDEVICECONTEXT_H
