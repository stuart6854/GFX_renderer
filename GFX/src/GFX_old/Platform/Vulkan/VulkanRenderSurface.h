#pragma once

#include "GFX/Config.h"

#include "vk_mem_alloc.h"

#include <vulkan/vulkan.hpp>

namespace gfxOld
{
    class IWindowSurface;
    class RenderContext;

    class RenderSurface
    {
    public:
        explicit RenderSurface(IWindowSurface& windowSurface);
        ~RenderSurface();

        auto GetWidth() const -> uint32_t { return m_swapChainWidth; }
        auto GetHeight() const -> uint32_t { return m_swapChainHeight; }
        auto GetRenderPass() const -> vk::RenderPass { return m_renderPass; }
        auto GetCurrentFramebuffer() const -> vk::Framebuffer { return m_swapChainImages.at(m_swapChainImageIndex).Framebuffer; }

        void Resize(uint32_t width, uint32_t height);

        void NewFrame();

        void Submit(RenderContext& context);

        void Present();

        auto GetFrameIndex() const -> uint32_t { return m_frameIndex; }

    private:
        struct Frame
        {
            vk::Semaphore PresentComplete, RenderComplete;
            vk::Fence RenderFence;
        };

        auto GetCurrentFrame() -> Frame& { return m_frames.at(m_frameIndex); }

        void CreateSwapChain(uint32_t width, uint32_t height);

        void CreateFrameResources();

        void CreateRenderPass();
        void CreateDepthStencil();
        void CreateImageResources(const std::vector<vk::Image>& swapChainImages);

        void DestroySwapChainResources();
        void Destroy();

    private:
        struct SwapChainImage
        {
            vk::Image Image = {};
            vk::ImageView View = {};
            VmaAllocation Allocation = {};
            vk::Framebuffer Framebuffer = {};
        };

        vk::SurfaceKHR m_surface = {};
        vk::SwapchainKHR m_swapChain = {};
        uint32_t m_swapChainWidth = 0;
        uint32_t m_swapChainHeight = 0;
        vk::Format m_swapChainFormat = {};

        uint32_t m_swapChainImageIndex = 0;
        std::vector<SwapChainImage> m_swapChainImages = {};
        SwapChainImage m_swapChainDepthStencil = {};

        vk::RenderPass m_renderPass;

        std::array<Frame, Config::FramesInFlight> m_frames = {};
        uint32_t m_frameIndex = 0;
    };

}  // namespace gfxOld
