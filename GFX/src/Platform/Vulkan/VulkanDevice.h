#pragma once

#include "VulkanPhysicalDevice.h"

#include <vulkan/vulkan.hpp>

namespace gfx
{
    class VulkanDevice
    {
    public:
        VulkanDevice(VulkanPhysicalDevice& physicalDevice);
        ~VulkanDevice();

        auto GetHandle() -> vk::Device { return m_device; }
        auto GetGraphicsQueue() -> vk::Queue { return m_graphicsQueue; }

        void WaitForFence(vk::Fence fence);
        void WaitIdle();

        auto AcquireNextImage(vk::SwapchainKHR swapchain, vk::Semaphore semaphore) -> uint32_t;

        auto GetCommandBuffer(bool begin) -> vk::CommandBuffer;
        void FlushCommandBuffer(vk::CommandBuffer cmdBuffer);
        void FlushCommandBuffer(vk::CommandBuffer cmdBuffer, vk::Queue queue);

    private:
        VulkanPhysicalDevice& m_physicalDevice;
        vk::Device m_device;

        vk::Queue m_graphicsQueue;

        vk::CommandPool m_commandPool;
    };
}
