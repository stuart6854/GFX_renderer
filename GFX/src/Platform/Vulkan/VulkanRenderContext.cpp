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
        vk::RenderPassBeginInfo beginInfo{};
        beginInfo.renderPass = framebuffer->GetRenderPass();
        beginInfo.framebuffer = framebuffer->GetFramebuffer();
        beginInfo.renderArea.setOffset({ 0, 0 });
        beginInfo.renderArea.extent.setWidth(framebuffer->GetWidth());
        beginInfo.renderArea.extent.setHeight(framebuffer->GetHeight());
        beginInfo.setClearValues(framebuffer->GetClearValues());

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

    void RenderContext::BindDescriptorSets(vk::PipelineBindPoint bindPoint,
                                           vk::PipelineLayout layout,
                                           uint32_t firstSet,
                                           const std::vector<vk::DescriptorSet>& sets,
                                           const std::vector<uint32_t>& dynamicOffsets)
    {
        GetCommandBuffer().BindDescriptorSets(bindPoint, layout, firstSet, sets, dynamicOffsets);
    }

    void RenderContext::Draw(uint32_t vertexCount) { GetCommandBuffer().Draw(vertexCount); }

    void RenderContext::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance)
    {
        GetCommandBuffer().DrawIndexed(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    }

    void RenderContext::NextCommandBuffer()
    {
        const auto& framesInFlight = Config::FramesInFlight;
        m_activeCmdBufferIndex = (m_activeCmdBufferIndex + 1) % framesInFlight;
    }

    auto RenderContext::GetCommandBuffer() -> CommandBuffer& { return m_cmdBuffers.at(m_activeCmdBufferIndex); }

}  // namespace gfx

#endif