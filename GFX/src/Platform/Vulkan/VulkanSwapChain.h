#include "GFX/Core/SwapChain.h"

#include "GFX/Config.h"
#include "vk_mem_alloc.h"

#include <vulkan/vulkan.hpp>

#include <cstdint>
#include <array>
#include <vector>

namespace gfx
{
    class VulkanSwapChain : public SwapChain
    {
    public:
        VulkanSwapChain(Window* window);
        ~VulkanSwapChain();

        auto GetWidth() const -> uint32_t override { return m_width; }
        auto GetHeight() const -> uint32_t override { return m_height; }

        auto GetRenderPass() const -> vk::RenderPass { return m_renderPass; }

        auto GetCurrentFramebuffer() const -> vk::Framebuffer { return m_framebuffers.at(m_imageIndex); }

        void Recreate(uint32_t width, uint32_t height) override;

        void NewFrame() override;
        void Present(CommandBuffer* cmdBuffer) override;

    private:
        struct Frame
        {
            vk::Semaphore PresentComplete, RenderComplete;
            vk::Fence RenderFence;
        };

        auto GetCurrentFrame() -> Frame& { return m_frames.at(m_frameIndex); }

        void CreateFrameResources();

        void CreateRenderPass();
        void CreateDepthStencil();
        void CreateImageResources();

        void DestroySwapChainResources();
        void Destroy();

    private:
        Window* m_window;

        vk::SurfaceKHR m_surface;
        vk::SwapchainKHR m_swapChain;
        uint32_t m_imageIndex = 0;
        uint32_t m_width;
        uint32_t m_height;
        vk::Format m_format;

        std::vector<vk::Image> m_images;
        std::vector<vk::ImageView> m_imageViews;
        std::vector<vk::Framebuffer> m_framebuffers;

        struct DepthStencil
        {
            vk::Image Image;
            vk::ImageView View;
            VmaAllocation Allocation;
        } m_depthStencil;

        vk::RenderPass m_renderPass;

        std::array<Frame, Config::FramesInFlight> m_frames = {};
        uint32_t m_frameIndex = 0;
    };
}
