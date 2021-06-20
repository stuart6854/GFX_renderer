//
// Created by stumi on 07/06/21.
//

#ifdef GFX_API_VULKAN

    #include "VulkanRenderContext.h"

    #include "VulkanCore.h"

    #include "GFX/Config.h"
    #include "GFX/Resources/Framebuffer.h"

namespace gfx
{
    void RenderContext::Begin() { GetCommandBuffer().Begin(); }

    void RenderContext::End() { GetCommandBuffer().End(); }

    void RenderContext::BeginRenderPass(const Color& clearColor, Framebuffer* framebuffer)
    {
        vk::ClearValue colorClear;
        colorClear.setColor({ clearColor.ToArray() });

        vk::ClearValue depthClear;
        depthClear.setDepthStencil({ 1.0f, 0 });

        auto clearValues = { colorClear, depthClear };

        vk::RenderPassBeginInfo beginInfo{};
        beginInfo.renderPass = framebuffer->GetRenderPass();
        beginInfo.framebuffer = framebuffer->GetFramebuffer();
        beginInfo.renderArea.setOffset({ 0, 0 });
        beginInfo.renderArea.extent = framebuffer->GetExtent();
        beginInfo.setClearValues(clearValues);

        GetCommandBuffer().BeginRenderPass(beginInfo);
    }

    void RenderContext::EndRenderPass() { GetCommandBuffer().EndRenderPass(); }

    void RenderContext::BindPipeline(Pipeline* pipeline)
    {
        GetCommandBuffer().BindPipeline(pipeline);
        m_boundPipeline = pipeline;
    }

    void RenderContext::BindVertexBuffer(Buffer* buffer) { GetCommandBuffer().BindVertexBuffer(buffer); }

    void RenderContext::BindIndexBuffer(Buffer* buffer) { GetCommandBuffer().BindIndexBuffer(buffer); }

    void RenderContext::PushConstants(ShaderStage stage, uint32_t offset, uint32_t size, const void* data)
    {
        GetCommandBuffer().PushConstants(m_boundPipeline->GetAPIPipelineLayout(), stage, offset, size, data);
    }

    void RenderContext::Draw(uint32_t vertexCount) { GetCommandBuffer().Draw(vertexCount); }

    void RenderContext::DrawIndexed(uint32_t indexCount) { GetCommandBuffer().DrawIndexed(indexCount); }

    void RenderContext::NextCommandBuffer()
    {
        const auto& framesInFlight = Config::FramesInFlight;
        m_activeCmdBufferIndex = (m_activeCmdBufferIndex + 1) % framesInFlight;
    }

    auto RenderContext::GetCommandBuffer() -> CommandBuffer& { return m_cmdBuffers.at(m_activeCmdBufferIndex); }

}  // namespace gfx

#endif