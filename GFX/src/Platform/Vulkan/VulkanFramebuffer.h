#pragma once

#include "GFX/Resources/Framebuffer.h"

#include <vulkan/vulkan.hpp>

#include <vector>

namespace gfx
{
    class VulkanFramebuffer : public Framebuffer
    {
    public:
        VulkanFramebuffer(SwapChain* swapChain);
        VulkanFramebuffer(const FramebufferDesc& desc);
        ~VulkanFramebuffer();

        auto GetColorAttachmentCount() const -> uint32_t override { return (uint32_t)m_attachmentTextures.size(); }

        auto GetColorTexture(uint32_t index) const -> Texture* override { return m_attachmentTextures[index].get(); }
        auto GetDepthTexture() const -> Texture* override { return m_depthAttachentTexture.get(); }

        auto GetBeginInfo() const -> vk::RenderPassBeginInfo;

        auto GetRenderPass() const -> vk::RenderPass { return m_renderPass; }
        auto GetHandle() const -> vk::Framebuffer { return m_framebuffer; }

        void Resize(uint32_t width, uint32_t height, bool forceRecreate = false) override;

    private:
        void Invalidate() override;

    private:
        FramebufferDesc m_desc;

        std::vector<OwnedPtr<Texture>> m_attachmentTextures;
        OwnedPtr<Texture> m_depthAttachentTexture;

        std::vector<vk::ClearValue> m_clearValues;

        vk::RenderPass m_renderPass;
        vk::Framebuffer m_framebuffer;
    };
}
