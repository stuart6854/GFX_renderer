//
// Created by stumi on 09/06/21.
//

#ifdef GFX_API_VULKAN

    #include "GFX/Debug.h"

    #include "VulkanCore.h"
    #include "VulkanCommandBuffer.h"

    #include <iostream>

namespace gfx
{
    static auto ShaderStageToVulkan(ShaderStage stage) -> vk::ShaderStageFlagBits
    {
        switch (stage)
        {
            case ShaderStage::eNone: break;
            case ShaderStage::eVertex: return vk::ShaderStageFlagBits::eVertex;
            case ShaderStage::ePixel: return vk::ShaderStageFlagBits::eFragment;
        }
        return {};
    }

    CommandBuffer::CommandBuffer()
    {
        auto device = Vulkan::GetDevice();

        // Command Pool
        vk::CommandPoolCreateInfo poolInfo{};
        poolInfo.flags = vk::CommandPoolCreateFlagBits::eTransient | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
        poolInfo.queueFamilyIndex = Vulkan::GetGraphicsQueueFamily();

        m_cmdPool = device.createCommandPool(poolInfo);

        // Command Buffer
        vk::CommandBufferAllocateInfo bufferInfo{};
        bufferInfo.level = vk::CommandBufferLevel::ePrimary;
        bufferInfo.commandBufferCount = 1;
        bufferInfo.commandPool = m_cmdPool;

        m_cmdBuffer = device.allocateCommandBuffers(bufferInfo).at(0);

        vk::FenceCreateInfo fenceInfo{};
        fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);

        m_fence = device.createFence(fenceInfo);
    }

    CommandBuffer::~CommandBuffer()
    {
        auto device = Vulkan::GetDevice();

        device.destroy(m_cmdPool);
    }

    void CommandBuffer::Begin()
    {
        auto device = Vulkan::GetDevice();

        // Wait until GPU has finished rendering the last frame
        device.waitForFences(m_fence, VK_TRUE, UINT64_MAX);
        device.resetFences(m_fence);

        vk::CommandBufferBeginInfo beginInfo{};

        m_cmdBuffer.begin(beginInfo);
    }

    void CommandBuffer::End() { m_cmdBuffer.end(); }

    void CommandBuffer::BeginRenderPass(vk::RenderPassBeginInfo& beginInfo)
    {
        m_cmdBuffer.beginRenderPass(beginInfo, vk::SubpassContents::eInline);
        vk::Viewport viewport;
        viewport.width = 720;
        viewport.height = 480;

        vk::Rect2D scissor;
        scissor.extent.width = 720;
        scissor.extent.height = 480;

        m_cmdBuffer.setViewport(0, viewport);
        m_cmdBuffer.setScissor(0, scissor);
    }

    void CommandBuffer::EndRenderPass() { m_cmdBuffer.endRenderPass(); }

    void CommandBuffer::BindPipeline(Pipeline* pipeline) { m_cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline->GetAPIPipeline()); }

    void CommandBuffer::BindVertexBuffer(Buffer* buffer)
    {
        if (buffer->GetType() != BufferType::eVertex)
        {
            GFX_ERROR("Trying to bind a buffer that is not a Vertex buffer!");
            return;
        }

        auto apiBuffer = buffer->GetAPIBuffer();

        m_cmdBuffer.bindVertexBuffers(0, apiBuffer, { 0 });
    }

    void CommandBuffer::BindIndexBuffer(Buffer* buffer)
    {
        if (buffer->GetType() != BufferType::eIndex)
        {
            GFX_ERROR("Trying to bind a buffer that is not a Index buffer!");
            return;
        }

        auto apiBuffer = buffer->GetAPIBuffer();

        m_cmdBuffer.bindIndexBuffer(apiBuffer, 0, vk::IndexType::eUint32);
    }

    void CommandBuffer::PushConstants(vk::PipelineLayout layout, ShaderStage stage, uint32_t offset, uint32_t size, const void* data)
    {
        m_cmdBuffer.pushConstants(layout, ShaderStageToVulkan(stage), offset, size, data);
    }

    void CommandBuffer::Draw(uint32_t vertexCount) { m_cmdBuffer.draw(vertexCount, 1, 0, 0); }

    void CommandBuffer::DrawIndexed(uint32_t indexCount) { m_cmdBuffer.drawIndexed(indexCount, 1, 0, 0, 0); }

    void CommandBuffer::CopyBuffer(vk::Buffer src, vk::Buffer dst, uint32_t size)
    {
        vk::BufferCopy copyRegion{};
        copyRegion.setSize(size);
        m_cmdBuffer.copyBuffer(src, dst, copyRegion);
    }

    auto CommandBuffer::GetAPIResource() -> vk::CommandBuffer { return m_cmdBuffer; }

    void CommandBuffer::SetAPIResource(vk::CommandBuffer cmdBuffer) { m_cmdBuffer = cmdBuffer; }

    auto CommandBuffer::GetFence() -> vk::Fence { return m_fence; }

}  // namespace gfx

#endif