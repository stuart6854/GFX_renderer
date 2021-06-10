//
// Created by stumi on 07/06/21.
//

#ifdef GFX_API_VULKAN

    #include <GFX/Resources/Framebuffer.h>
    #include "VulkanRenderContext.h"

namespace gfx
{
    void RenderContext::Begin() { m_cmdBuffer.Begin(); }

    void RenderContext::End() { m_cmdBuffer.End(); }

    void RenderContext::BeginRenderPass(Color clearColor, Framebuffer framebuffer)
    {
        vk::ClearValue colorClear;
        colorClear.setColor({ clearColor.ToArray() });

        vk::ClearValue depthClear;
        depthClear.setDepthStencil({ 1.0f, 0 });

        auto clearValues = { colorClear, depthClear };

        vk::RenderPassBeginInfo beginInfo{};
        beginInfo.renderPass = framebuffer.GetRenderPass();
        beginInfo.framebuffer = framebuffer.GetFramebuffer();
        beginInfo.renderArea.setOffset({ 0, 0 });
        beginInfo.renderArea.extent = framebuffer.GetExtent();
        beginInfo.setClearValues(clearValues);

        m_cmdBuffer.BeginRenderPass(beginInfo);
    }

    void RenderContext::EndRenderPass() { m_cmdBuffer.EndRenderPass(); }

    void RenderContext::SetVertexBuffer() {}

    void RenderContext::SetIndexBuffer() {}

    void RenderContext::Draw() {}

    void RenderContext::DrawIndexed() {}

    auto RenderContext::GetCommandBuffer() -> CommandBuffer& { return m_cmdBuffer; }

}  // namespace gfx

#endif