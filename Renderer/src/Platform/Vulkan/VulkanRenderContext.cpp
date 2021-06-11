//
// Created by stumi on 07/06/21.
//

#ifdef GFX_API_VULKAN

    #include "GFX/Resources/Framebuffer.h"

    #include "VulkanCore.h"
    #include "VulkanRenderContext.h"

namespace gfx
{
    void RenderContext::Begin() { GetCommandBuffer().Begin(); }

    void RenderContext::End() { GetCommandBuffer().End(); }

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

        GetCommandBuffer().BeginRenderPass(beginInfo);
    }

    void RenderContext::EndRenderPass() { GetCommandBuffer().EndRenderPass(); }

    void RenderContext::SetVertexBuffer() {}

    void RenderContext::SetIndexBuffer() {}

    void RenderContext::Draw() {}

    void RenderContext::DrawIndexed() {}

    void RenderContext::NextCommandBuffer() { m_activeCmdBufferIndex = (m_activeCmdBufferIndex + 1) % FRAME_OVERLAP; }

    auto RenderContext::GetCommandBuffer() -> CommandBuffer& { return m_cmdBuffers.at(m_activeCmdBufferIndex); }

}  // namespace gfx

#endif