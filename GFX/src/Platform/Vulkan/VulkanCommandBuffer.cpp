﻿#include "VulkanCommandBuffer.h"

#include "GFX/Config.h"
#include "VulkanBackend.h"
#include "VulkanDevice.h"
#include "VulkanFramebuffer.h"

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

    void VulkanCommandBuffer::BeginRenderPass(Framebuffer* framebuffer)
    {
        auto* vkFramebuffer = static_cast<VulkanFramebuffer*>(framebuffer);
        m_currentCmdBuffer.beginRenderPass(vkFramebuffer->GetBeginInfo(), vk::SubpassContents::eInline);
    }

    void VulkanCommandBuffer::EndRenderPass()
    {
        m_currentCmdBuffer.endRenderPass();
    }
}
