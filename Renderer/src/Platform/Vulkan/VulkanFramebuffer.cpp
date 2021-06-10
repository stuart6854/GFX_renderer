//
// Created by stumi on 10/06/21.
//

#ifdef GFX_API_VULKAN

    #include "VulkanFramebuffer.h"

    #include <utility>

namespace gfx
{
    Framebuffer::Framebuffer(vk::Framebuffer framebuffer, vk::RenderPass renderpass, vk::Extent2D extent)
        : m_framebuffer(framebuffer), m_renderpass(renderpass), m_extent(extent)
    {
    }

    auto Framebuffer::GetFramebuffer() const -> vk::Framebuffer { return m_framebuffer; }

    auto Framebuffer::GetRenderPass() const -> vk::RenderPass { return m_renderpass; }

    auto Framebuffer::GetExtent() const -> vk::Extent2D { return m_extent; }

}  // namespace gfx

#endif