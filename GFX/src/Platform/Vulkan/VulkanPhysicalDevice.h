#pragma once

#include "GFX/Core/Base.h"

#include <vulkan/vulkan.hpp>

namespace gfx
{
    class VulkanPhysicalDevice
    {
    public:
        struct QueueFamilyIndices
        {
            int32_t Graphics = -1;
            int32_t Transfer = -1;
            int32_t Compute = -1;
        };

    public:
        static auto Select(vk::Instance instance) -> OwnedPtr<VulkanPhysicalDevice>;

        VulkanPhysicalDevice(vk::PhysicalDevice physicalDevice);
        ~VulkanPhysicalDevice();

        auto GetHandle() const -> vk::PhysicalDevice { return m_physicalDevice; }

        auto GetDepthFormat() const -> vk::Format { return m_depthFormat; }
        auto GetQueueFamilyIndices() const -> const QueueFamilyIndices& { return m_queueFamilyIndices; }

        auto GetQueueCreateInfos() const -> const std::vector<vk::DeviceQueueCreateInfo>& { return m_queueCreateInfos; }

    private:
        auto FindDepthFormat() -> vk::Format;
        auto FindQueueFamilyIndices(vk::QueueFlags flags) -> QueueFamilyIndices;

    private:
        vk::PhysicalDevice m_physicalDevice;

        vk::PhysicalDeviceProperties m_properties;

        vk::Format m_depthFormat;
        QueueFamilyIndices m_queueFamilyIndices;

        std::vector<vk::DeviceQueueCreateInfo> m_queueCreateInfos;
    };
}
