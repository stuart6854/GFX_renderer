#include "VulkanFramebuffer.h"

#include "VulkanBackend.h"
#include "VulkanSwapChain.h"

namespace gfx
{
    VulkanFramebuffer::VulkanFramebuffer(SwapChain* swapChain)
        : Framebuffer(swapChain)
    {
        Resize(swapChain->GetWidth(), swapChain->GetHeight());
    }

    auto VulkanFramebuffer::GetBeginInfo() const -> vk::RenderPassBeginInfo
    {
        auto* vkSwapChain = static_cast<VulkanSwapChain*>(m_swapChain);

        vk::RenderPassBeginInfo beginInfo{};
        beginInfo.setClearValues(m_clearValues);
        beginInfo.setFramebuffer(!m_isSwapChainTarget ? m_framebuffer : vkSwapChain->GetCurrentFramebuffer());
        beginInfo.setRenderPass(m_renderPass);
        beginInfo.renderArea.offset.setX(0);
        beginInfo.renderArea.offset.setY(0);
        beginInfo.renderArea.extent.setWidth(m_width);
        beginInfo.renderArea.extent.setHeight(m_height);
        return beginInfo;
    }

    void VulkanFramebuffer::Resize(uint32_t width, uint32_t height)
    {
        if (m_width == width && m_height == height) return;

        m_width = width;
        m_height = height;

        if (!m_isSwapChainTarget)
        {
            Invalidate();
        }
        else
        {
            auto* vkSwapChain = static_cast<VulkanSwapChain*>(m_swapChain);

            m_renderPass = vkSwapChain->GetRenderPass();

            m_clearValues.clear();

            const std::array clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
            m_clearValues.emplace_back().setColor(clearColor);
            m_clearValues.emplace_back().setDepthStencil({ 1.0f, 0 });
        }
    }

    void VulkanFramebuffer::Invalidate()
    {
        auto backend = VulkanBackend::Get();
        auto device = backend->GetDevice();
        auto vkDevice = device.GetHandle();

        if (m_framebuffer)
        {
            vkDevice.destroy(m_framebuffer);

            // for (auto& image : m_attachmentImages)
            // {
            //     //TODO: Destroy image
            // }
            // if (m_depthAttachmentImage)
            // {
            //     //TODO: Destroy depth image
            // }
        }

        // RenderPass
    }
}
