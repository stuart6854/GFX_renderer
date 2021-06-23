//
// Created by stumi on 10/06/21.
//

#ifndef PERSONAL_RENDERER_VULKANFRAMEBUFFER_H
#define PERSONAL_RENDERER_VULKANFRAMEBUFFER_H

#include <vulkan/vulkan.hpp>

namespace gfx
{
    class Framebuffer
    {
    public:
        Framebuffer() = default;
        Framebuffer(vk::Framebuffer framebuffer, vk::RenderPass renderpass, vk::Extent2D extent);

        auto GetFramebuffer() const -> vk::Framebuffer;
        auto GetRenderPass() const -> vk::RenderPass;
        auto GetExtent() const -> vk::Extent2D;
        auto IsSwapchainTarget() const -> bool { return m_swapchainTarget; }

    private:
        vk::Framebuffer m_framebuffer;
        vk::RenderPass m_renderpass;
        vk::Extent2D m_extent;
        bool m_swapchainTarget = false;
    };
}  // namespace gfx

#endif  // PERSONAL_RENDERER_VULKANFRAMEBUFFER_H
