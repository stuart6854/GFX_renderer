#include "VulkanPhysicalDevice.h"

#include "GFX/Debug.h"

namespace gfx
{
    auto VulkanPhysicalDevice::Select(vk::Instance instance) -> OwnedPtr<VulkanPhysicalDevice>
    {
        auto possibleGPUs = instance.enumeratePhysicalDevices();

        vk::PhysicalDevice chosenGpu;
        for (const auto& gpu : possibleGPUs)
        {
            auto properties = gpu.getProperties();
            if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
            {
                chosenGpu = gpu;
            }
        }

        if (!chosenGpu)
        {
            chosenGpu = possibleGPUs.at(0);
        }

        return CreateOwned<VulkanPhysicalDevice>(chosenGpu);
    }

    VulkanPhysicalDevice::VulkanPhysicalDevice(vk::PhysicalDevice physicalDevice)
        : m_physicalDevice(physicalDevice)
    {
        m_properties = m_physicalDevice.getProperties();
        GFX_INFO("Physical Device: {}", m_properties.deviceName);

        m_depthFormat = FindDepthFormat();
        GFX_INFO("  Depth Format: {}", vk::to_string(m_depthFormat));

        auto requestedQueueTypes = vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eTransfer | vk::QueueFlagBits::eCompute;
        m_queueFamilyIndices = FindQueueFamilyIndices(requestedQueueTypes);

        static const float defaultQueuePriority = 1.0f;

        GFX_TRACE("  Chosen Queue Families");
        GFX_TRACE("    Graphics: {}", m_queueFamilyIndices.Graphics);
        GFX_TRACE("    Transfer: {}", m_queueFamilyIndices.Transfer);
        GFX_TRACE("    Compute: {}", m_queueFamilyIndices.Compute);

        // Graphics queue
        if (requestedQueueTypes & vk::QueueFlagBits::eGraphics)
        {
            vk::DeviceQueueCreateInfo queueInfo{};
            queueInfo.setQueueFamilyIndex(m_queueFamilyIndices.Graphics);
            queueInfo.setQueueCount(1);
            queueInfo.setPQueuePriorities(&defaultQueuePriority);
            m_queueCreateInfos.push_back(queueInfo);
        }

        // Dedicated Transfer queue
        if (requestedQueueTypes & vk::QueueFlagBits::eTransfer)
        {
            if ((m_queueFamilyIndices.Transfer != m_queueFamilyIndices.Graphics) && (m_queueFamilyIndices.Transfer != m_queueFamilyIndices.Compute))
            {
                // If transfer family index differs, we need an additional queue create info for the transfer queue
                vk::DeviceQueueCreateInfo queueInfo{};
                queueInfo.setQueueFamilyIndex(m_queueFamilyIndices.Transfer);
                queueInfo.setQueueCount(1);
                queueInfo.setPQueuePriorities(&defaultQueuePriority);
                m_queueCreateInfos.push_back(queueInfo);
            }
        }

        // Dedicated Compute queue
        if (requestedQueueTypes & vk::QueueFlagBits::eCompute)
        {
            if (m_queueFamilyIndices.Compute != m_queueFamilyIndices.Graphics)
            {
                // If compute family index differs, we need an additional queue create info for the compute queue
                vk::DeviceQueueCreateInfo queueInfo{};
                queueInfo.setQueueFamilyIndex(m_queueFamilyIndices.Compute);
                queueInfo.setQueueCount(1);
                queueInfo.setPQueuePriorities(&defaultQueuePriority);
                m_queueCreateInfos.push_back(queueInfo);
            }
        }
    }

    VulkanPhysicalDevice::~VulkanPhysicalDevice()
    {
    }

    auto VulkanPhysicalDevice::FindDepthFormat() -> vk::Format
    {
        // Since all depth formats may be optional, we need to find a suitable
        // depth format to use. Start with the highest precision packed format.
        std::vector<vk::Format> depthFormats = {
            vk::Format::eD32SfloatS8Uint, vk::Format::eD32Sfloat, vk::Format::eD24UnormS8Uint, vk::Format::eD16UnormS8Uint, vk::Format::eD16Unorm
        };

        for (auto& format : depthFormats)
        {
            vk::FormatProperties formatProps = m_physicalDevice.getFormatProperties(format);

            // Format must support depth stencil attachment for optimal tiling
            if (formatProps.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
            {
                return format;
            }
        }
        return vk::Format::eUndefined;
    }

    auto VulkanPhysicalDevice::FindQueueFamilyIndices(vk::QueueFlags flags) -> QueueFamilyIndices
    {
        QueueFamilyIndices indices;

        auto queueProperties = m_physicalDevice.getQueueFamilyProperties();

        // Dedicated queue for compute
        // Try to find a queue family index that supports compute but not graphics
        if (flags & vk::QueueFlagBits::eCompute)
        {
            for (uint32_t i = 0; i < queueProperties.size(); i++)
            {
                auto& queueFamilyProperties = queueProperties[i];
                if ((queueFamilyProperties.queueFlags & vk::QueueFlagBits::eCompute) && !(queueFamilyProperties.queueFlags & vk::QueueFlagBits::eGraphics))
                {
                    indices.Compute = i;
                    break;
                }
            }
        }

        // Dedicated queue for transfer
        // Try to find a queue family index that supports transfer but not graphics and compute
        if (flags & vk::QueueFlagBits::eTransfer)
        {
            for (uint32_t i = 0; i < queueProperties.size(); i++)
            {
                auto& queueFamilyProperties = queueProperties[i];
                if ((queueFamilyProperties.queueFlags & vk::QueueFlagBits::eTransfer) && !(queueFamilyProperties.queueFlags & vk::QueueFlagBits::eGraphics) && !
                    (queueFamilyProperties.queueFlags & vk::QueueFlagBits::eCompute))
                {
                    indices.Transfer = i;
                    break;
                }
            }
        }

        // For other queue types or if no separate compute queue is present, return the first one to support the requested flags
        for (uint32_t i = 0; i < queueProperties.size(); i++)
        {
            if ((flags & vk::QueueFlagBits::eTransfer) && indices.Transfer == -1)
            {
                if (queueProperties[i].queueFlags & vk::QueueFlagBits::eTransfer)
                    indices.Transfer = i;
            }

            if ((flags & vk::QueueFlagBits::eCompute) && indices.Compute == -1)
            {
                if (queueProperties[i].queueFlags & vk::QueueFlagBits::eCompute)
                    indices.Compute = i;
            }

            if (flags & vk::QueueFlagBits::eGraphics)
            {
                if (queueProperties[i].queueFlags & vk::QueueFlagBits::eGraphics)
                    indices.Graphics = i;
            }
        }

        return indices;
    }
}
