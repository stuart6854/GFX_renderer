//
// Created by stumi on 09/06/21.
//

#ifdef GFX_API_VULKAN

    #include "VulkanCommandBuffer.h"

    #include "VulkanCore.h"

    #include <iostream>

namespace gfx
{
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

    void CommandBuffer::BeginRenderPass(vk::RenderPassBeginInfo& beginInfo) { m_cmdBuffer.beginRenderPass(beginInfo, vk::SubpassContents::eInline); }

    void CommandBuffer::EndRenderPass() { m_cmdBuffer.endRenderPass(); }

    void CommandBuffer::BindVertexBuffer(Buffer& buffer)
    {
        if (buffer.GetType() != BufferType::eVertex)
        {
            std::cout << "Trying to bind a buffer that is not a Vertex buffer!" << std::endl;
            return;
        }

        auto apiBuffer = buffer.GetAPIResource();

        m_cmdBuffer.bindVertexBuffers(0, apiBuffer, {});
    }

    void CommandBuffer::BindIndexBuffer(Buffer& buffer)
    {
        if (buffer.GetType() != BufferType::eIndex)
        {
            std::cout << "Trying to bind a buffer that is not a Index buffer!" << std::endl;
            return;
        }

        auto apiBuffer = buffer.GetAPIResource();

        m_cmdBuffer.bindIndexBuffer(apiBuffer, 0, vk::IndexType::eUint32);
    }

    auto CommandBuffer::GetAPIResource() -> vk::CommandBuffer { return m_cmdBuffer; }

    void CommandBuffer::SetAPIResource(vk::CommandBuffer cmdBuffer) { m_cmdBuffer = cmdBuffer; }

    auto CommandBuffer::GetFence() -> vk::Fence { return m_fence; }

}  // namespace gfx

#endif