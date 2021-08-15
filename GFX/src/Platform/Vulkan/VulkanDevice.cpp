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
        features.fillModeNonSolid = true;
        features.wideLines = true;
        features.samplerAnisotropy = true;
        features.shaderSampledImageArrayDynamicIndexing = true;

        deviceInfo.setPEnabledFeatures(&features);

        m_device = m_physicalDevice.GetHandle().createDevice(deviceInfo);

        auto queueFamilyIndices = m_physicalDevice.GetQueueFamilyIndices();
        m_graphicsQueue = m_device.getQueue(queueFamilyIndices.Graphics, 0);
        //        m_transferQueue = m_device.getQueue(m_transferQueueFamily, 0);

        vk::CommandPoolCreateInfo poolInfo{};
        poolInfo.setQueueFamilyIndex(queueFamilyIndices.Graphics);
        poolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

        {
            m_commandPool = m_device.createCommandPool(poolInfo);

            std::vector<vk::DescriptorPoolSize> poolSizes = {
                { vk::DescriptorType::eSampler, 1000 },
                { vk::DescriptorType::eCombinedImageSampler, 1000 },
                { vk::DescriptorType::eSampledImage, 1000 },
                { vk::DescriptorType::eStorageImage, 1000 },
                { vk::DescriptorType::eUniformTexelBuffer, 1000 },
                { vk::DescriptorType::eStorageTexelBuffer, 1000 },
                { vk::DescriptorType::eUniformBuffer, 1000 },
                { vk::DescriptorType::eStorageBuffer, 1000 },
                { vk::DescriptorType::eUniformBufferDynamic, 1000 },
                { vk::DescriptorType::eStorageBufferDynamic, 1000 },
                { vk::DescriptorType::eInputAttachment, 1000 },
            };

            vk::DescriptorPoolCreateInfo poolInfo{};
            poolInfo.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);
            poolInfo.setPoolSizes(poolSizes);
            poolInfo.setMaxSets(1000);

            for (auto& pool : m_descriptorPools)
            {
                pool = m_device.createDescriptorPool(poolInfo);
            }
        }
    }

    VulkanDevice::~VulkanDevice()
    {
        m_device.waitIdle();

        for (const auto& pool : m_descriptorPools)
            m_device.destroy(pool);

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

    void VulkanDevice::ResetFence(vk::Fence fence)
    {
        m_device.resetFences(fence);
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

    auto VulkanDevice::AllocateDescriptorSet(const uint32_t frameIndex,
                                             vk::DescriptorSetLayout setLayout) -> vk::DescriptorSet
    {
        vk::DescriptorSetAllocateInfo allocInfo{};
        allocInfo.setDescriptorPool(m_descriptorPools[frameIndex]);
        allocInfo.setDescriptorSetCount(1);
        allocInfo.setSetLayouts(setLayout);

        return m_device.allocateDescriptorSets(allocInfo)[0];
    }

    void VulkanDevice::ResetDescriptorPool(const uint32_t frameIndex) const
    {
        m_device.resetDescriptorPool(m_descriptorPools[frameIndex]);
    }
}
