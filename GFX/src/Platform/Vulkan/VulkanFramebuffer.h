﻿#pragma once

#include "GFX/Resources/Framebuffer.h"

#include <vulkan/vulkan.hpp>

namespace gfx
{
    class VulkanFramebuffer : public Framebuffer
    {
    public:
        VulkanFramebuffer(SwapChain* swapChain);

        auto GetBeginInfo() const -> vk::RenderPassBeginInfo;

        auto GetRenderPass() const -> vk::RenderPass { return m_renderPass; }
        auto GetHandle() const -> vk::Framebuffer { return m_framebuffer; }

        void Resize(uint32_t width, uint32_t height) override;

    private:
        void Invalidate() override;

    private:
        std::vector<vk::ClearValue> m_clearValues;

        vk::RenderPass m_renderPass;
        vk::Framebuffer m_framebuffer;
    };
}
