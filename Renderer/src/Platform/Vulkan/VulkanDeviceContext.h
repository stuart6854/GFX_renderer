//
// Created by stumi on 07/06/21.
//

#ifndef PERSONAL_RENDERER_VULKANDEVICECONTEXT_H
#define PERSONAL_RENDERER_VULKANDEVICECONTEXT_H

#include "VulkanSurface.h"

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
        DeviceContext() = default;
        ~DeviceContext();

        void ProcessWindowChanges(ISurface& surface, uint32_t windowWidth, uint32_t windowHeight);

        void Submit(RenderContext& context);
        void Present();

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

        struct Frame
        {
            vk::Semaphore PresentComplete, RenderComplete;
            vk::Fence RenderFence;

            vk::CommandPool CmdPool;
            vk::CommandBuffer MainCmdBuffer;
        };
        std::array<Frame, FRAME_OVERLAP> m_frames;
        uint32_t m_frameCounter;

        auto GetCurrentFrame() -> Frame&;
    };
}  // namespace gfx

#endif  // PERSONAL_RENDERER_VULKANDEVICECONTEXT_H
