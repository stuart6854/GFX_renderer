//
// Created by stumi on 10/06/21.
//

#ifndef PERSONAL_RENDERER_VULKANFRAMEBUFFER_H
#define PERSONAL_RENDERER_VULKANFRAMEBUFFER_H

#include "GFX/Resources/ResourceDescriptions.h"

#include "VulkanImage.h"

namespace gfx
{
    class RenderSurface;

    class Framebuffer
    {
    public:
        explicit Framebuffer(RenderSurface* renderSurface);
        explicit Framebuffer(const FramebufferDesc& desc);

        auto GetWidth() const -> uint32_t { return m_desc.Width; }
        auto GetHeight() const -> uint32_t { return m_desc.Height; }
        auto IsSwapchainTarget() const -> bool { return m_desc.IsSwapChainTarget; }

        void Resize(uint32_t width, uint32_t height, bool forceRecreate = false);

        auto GetRenderPass() const -> vk::RenderPass { return m_renderPass; }
        auto GetFramebuffer() const -> vk::Framebuffer;

    private:
        void Invalidate();

    private:
        FramebufferDesc m_desc;

        std::vector<std::shared_ptr<Image>> m_attachmentImages;
        std::shared_ptr<Image> m_depthAttachmentImage;

        vk::RenderPass m_renderPass;
        vk::Framebuffer m_framebuffer;
    };
}  // namespace gfx

#endif  // PERSONAL_RENDERER_VULKANFRAMEBUFFER_H
