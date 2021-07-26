#include "VulkanCommandBuffer.h"

#include "GFX/Debug.h"

#include "GFX/Config.h"
#include "VulkanBackend.h"
#include "VulkanDevice.h"
#include "VulkanFramebuffer.h"
#include "VulkanPipeline.h"
#include "VulkanBuffer.h"
#include "VulkanResourceSet.h"
#include "VulkanUtils.h"

#include <vector>

namespace gfx
{
    VulkanCommandBuffer::VulkanCommandBuffer(uint32_t count)
    {
        auto* backend = VulkanBackend::Get();
        auto& gpu = backend->GetPhysicalDevice();
        auto& device = backend->GetDevice();
        auto vkDevice = device.GetHandle();

        if (count == 0)
            count = Config::FramesInFlight;

        vk::CommandPoolCreateInfo poolInfo{};
        poolInfo.setQueueFamilyIndex(gpu.GetQueueFamilyIndices().Graphics);
        poolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

        m_cmdPool = vkDevice.createCommandPool(poolInfo);

        vk::CommandBufferAllocateInfo allocInfo{};
        allocInfo.setCommandPool(m_cmdPool);
        allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
        allocInfo.setCommandBufferCount(count);

        m_cmdBuffers = vkDevice.allocateCommandBuffers(allocInfo);

        vk::FenceCreateInfo fenceInfo{};
        fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
        m_fences.resize(count);
        for (auto& fence : m_fences)
            fence = vkDevice.createFence(fenceInfo);
    }

    VulkanCommandBuffer::~VulkanCommandBuffer()
    {
        auto* backend = VulkanBackend::Get();
        auto& device = backend->GetDevice();
        auto vkDevice = device.GetHandle();

        for (auto& fence : m_fences)
        {
            device.WaitForFence(fence);
            vkDevice.destroy(fence);
        }

        vkDevice.destroy(m_cmdPool);
    }

    void VulkanCommandBuffer::Begin()
    {
        auto* backend = VulkanBackend::Get();
        auto& device = backend->GetDevice();

        m_currentCmdBuffer = m_cmdBuffers[m_index];
        m_currentFence = m_fences[m_index];

        device.WaitForFence(m_currentFence);

        device.ResetFence(m_currentFence);

        vk::CommandBufferBeginInfo beginInfo{};
        m_currentCmdBuffer.begin(beginInfo);
    }

    void VulkanCommandBuffer::End()
    {
        m_currentCmdBuffer.end();

        m_index = (m_index + 1) % m_cmdBuffers.size();
    }

    void VulkanCommandBuffer::SetViewport(const Viewport& viewport)
    {
        vk::Viewport vp{};
        vp.setX(viewport.X);
        vp.setY(viewport.Y);
        vp.setWidth(viewport.Width);
        vp.setHeight(viewport.Height);
        vp.setMinDepth(viewport.MinDepth);
        vp.setMaxDepth(viewport.MaxDepth);

        m_currentCmdBuffer.setViewport(0, vp);
    }

    void VulkanCommandBuffer::SetScissor(const Scissor& scissor)
    {
        vk::Rect2D s{};
        s.offset.setX(scissor.X);
        s.offset.setY(scissor.Y);
        s.extent.setWidth(scissor.Width);
        s.extent.setHeight(scissor.Height);

        m_currentCmdBuffer.setScissor(0, s);
    }

    void VulkanCommandBuffer::BeginRenderPass(Framebuffer* framebuffer)
    {
        auto* vkFramebuffer = static_cast<VulkanFramebuffer*>(framebuffer);
        m_currentCmdBuffer.beginRenderPass(vkFramebuffer->GetBeginInfo(), vk::SubpassContents::eInline);
    }

    void VulkanCommandBuffer::EndRenderPass()
    {
        m_currentCmdBuffer.endRenderPass();
    }

    void VulkanCommandBuffer::BindPipeline(Pipeline* pipeline)
    {
        auto* vkPipeline = static_cast<VulkanPipeline*>(pipeline);
        m_currentCmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, vkPipeline->GetPipelineHandle());

        m_boundPipeline = vkPipeline;
    }

    void VulkanCommandBuffer::BindVertexBuffer(Buffer* buffer)
    {
        auto* vkBuffer = static_cast<VulkanBuffer*>(buffer);
        m_currentCmdBuffer.bindVertexBuffers(0, vkBuffer->GetHandle(), { 0 });
    }

    void VulkanCommandBuffer::BindIndexBuffer(Buffer* buffer)
    {
        auto* vkBuffer = static_cast<VulkanBuffer*>(buffer);
        m_currentCmdBuffer.bindIndexBuffer(vkBuffer->GetHandle(), { 0 }, vk::IndexType::eUint32);
    }

    void VulkanCommandBuffer::SetConstants(ShaderStage shaderStage, uint32_t offset, uint32_t size, const void* data)
    {
        auto layout = m_boundPipeline->GetLayoutHandle();
        auto stage = VkUtils::ToVkShaderStage(shaderStage);

        m_currentCmdBuffer.pushConstants(layout, stage, offset, size, data);
    }

    void VulkanCommandBuffer::BindResourceSets(uint32_t firstSet, const std::vector<ResourceSet*> sets)
    {
        auto layout = m_boundPipeline->GetLayoutHandle();

        std::vector<vk::DescriptorSet> vkSets;
        vkSets.resize(sets.size());
        for (int i = 0; i < sets.size(); i++)
        {
            vkSets[i] = static_cast<const VulkanResourceSet*>(sets[i])->GetHandle();
        }

        m_currentCmdBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, layout, firstSet, vkSets, {});
    }

    void VulkanCommandBuffer::Draw(uint32_t vertexCount)
    {
        m_currentCmdBuffer.draw(vertexCount, 1, 0, 0);
    }

    void VulkanCommandBuffer::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance)
    {
        m_currentCmdBuffer.drawIndexed(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    }
}
