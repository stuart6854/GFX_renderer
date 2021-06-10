//
// Created by stumi on 09/06/21.
//

#ifdef GFX_API_VULKAN

#include "VulkanCommandBuffer.h"

#include "VulkanCore.h"

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
    }

    CommandBuffer::~CommandBuffer()
    {
        auto device = Vulkan::GetDevice();

        device.destroy(m_cmdPool);
    }

    void CommandBuffer::Begin()
    {
        vk::CommandBufferBeginInfo beginInfo{};

        m_cmdBuffer.begin(beginInfo);
    }

    void CommandBuffer::End() { m_cmdBuffer.end(); }

    void CommandBuffer::BeginRenderPass(vk::RenderPassBeginInfo& beginInfo) { m_cmdBuffer.beginRenderPass(beginInfo, vk::SubpassContents::eInline); }

    void CommandBuffer::EndRenderPass() { m_cmdBuffer.endRenderPass(); }

    auto CommandBuffer::GetAPIResource() -> vk::CommandBuffer { return m_cmdBuffer; }

    void CommandBuffer::SetAPIResource(vk::CommandBuffer cmdBuffer) { m_cmdBuffer = cmdBuffer; }

}  // namespace gfx

#endif