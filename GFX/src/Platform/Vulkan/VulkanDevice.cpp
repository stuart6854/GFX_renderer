#include "VulkanDevice.h"

#include <vector>

namespace gfx
{
    VulkanDevice::VulkanDevice(VulkanPhysicalDevice& physicalDevice)
        : m_physicalDevice(physicalDevice)
    {
        std::vector<const char*> extensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        };

        vk::DeviceCreateInfo deviceInfo{};
        deviceInfo.setPEnabledExtensionNames(extensions);

        const auto& queueCreateInfos = m_physicalDevice.GetQueueCreateInfos();
        deviceInfo.setQueueCreateInfos(queueCreateInfos);

        vk::PhysicalDeviceFeatures features{};

        deviceInfo.setPEnabledFeatures(&features);

        m_device = m_physicalDevice.GetHandle().createDevice(deviceInfo);

        auto queueFamilyIndices = m_physicalDevice.GetQueueFamilyIndices();
        m_graphicsQueue = m_device.getQueue(queueFamilyIndices.Graphics, 0);
        //        m_transferQueue = m_device.getQueue(m_transferQueueFamily, 0);

        vk::CommandPoolCreateInfo poolInfo{};
        poolInfo.setQueueFamilyIndex(queueFamilyIndices.Graphics);
        poolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

        m_commandPool = m_device.createCommandPool(poolInfo);
    }

    VulkanDevice::~VulkanDevice()
    {
        m_device.waitIdle();

        m_device.destroy(m_commandPool);
        m_device.destroy();
    }

    void VulkanDevice::WaitForFence(vk::Fence fence)
    {
        m_device.waitForFences(fence, true, UINT64_MAX);
    }

    void VulkanDevice::WaitIdle()
    {
        m_device.waitIdle();
    }

    auto VulkanDevice::AcquireNextImage(vk::SwapchainKHR swapchain, vk::Semaphore semaphore) -> uint32_t
    {
        return m_device.acquireNextImageKHR(swapchain, UINT64_MAX, semaphore).value;
    }

    auto VulkanDevice::GetCommandBuffer(bool begin) -> vk::CommandBuffer
    {
        vk::CommandBufferAllocateInfo allocInfo{};
        allocInfo.setCommandPool(m_commandPool);
        allocInfo.setCommandBufferCount(1);
        allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);

        auto cmdBuffer = m_device.allocateCommandBuffers(allocInfo)[0];

        if (begin)
        {
            vk::CommandBufferBeginInfo beginInfo{};
            cmdBuffer.begin(beginInfo);
        }

        return cmdBuffer;
    }

    void VulkanDevice::FlushCommandBuffer(vk::CommandBuffer cmdBuffer)
    {
        FlushCommandBuffer(cmdBuffer, m_graphicsQueue);
    }

    void VulkanDevice::FlushCommandBuffer(vk::CommandBuffer cmdBuffer, vk::Queue queue)
    {
        cmdBuffer.end();

        vk::SubmitInfo submitInfo{};
        submitInfo.setCommandBuffers(cmdBuffer);

        vk::FenceCreateInfo fenceInfo{};
        auto fence = m_device.createFence(fenceInfo);

        queue.submit(submitInfo, fence);

        m_device.waitForFences(fence, true, UINT64_MAX);

        m_device.destroy(fence);
        m_device.free(m_commandPool, cmdBuffer);
    }
}
