//
// Created by stumi on 10/06/21.
//

#ifndef PERSONAL_RENDERER_VULKANFRAMEBUFFER_H
#define PERSONAL_RENDERER_VULKANFRAMEBUFFER_H

#include "GFX/Resources/ResourceDescriptions.h"

namespace gfx
{
    class RenderSurface;

    class Framebuffer
    {
    public:
        Framebuffer() = default;
        Framebuffer(RenderSurface* renderSurface);
        Framebuffer(const FramebufferDesc& desc);

        auto GetRenderPass() const -> vk::RenderPass { return m_renderPass; }
        auto IsSwapchainTarget() const -> bool { return m_desc.IsSwapChainTarget; }

        void Resize(uint32_t width, uint32_t height, bool forceRecreate = false);

    private:
        FramebufferDesc m_desc;
        vk::RenderPass m_renderPass;
    };
}  // namespace gfx

#endif  // PERSONAL_RENDERER_VULKANFRAMEBUFFER_H
