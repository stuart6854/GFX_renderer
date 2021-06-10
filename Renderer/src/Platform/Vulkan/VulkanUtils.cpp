//
// Created by stumi on 07/06/21.
//

#include "VulkanUtils.h"

namespace gfx::Vulkan
{
    auto ChooseSurfaceFormat(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface) -> vk::SurfaceFormatKHR
    {
        auto surfaceFormats = physicalDevice.getSurfaceFormatsKHR(surface);

        for (const auto& format : surfaceFormats)
        {
            if (format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
            {
                return format;
            }
        }
        return surfaceFormats[0];
    }

    auto ChoosePresentMode(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface) -> vk::PresentModeKHR
    {
        auto presentModes = physicalDevice.getSurfacePresentModesKHR(surface);

        for (const auto& mode : presentModes)
        {
            if (mode == vk::PresentModeKHR::eMailbox)
            {
                return mode;
            }
        }

        // Default to double buffering (VSync)
        return vk::PresentModeKHR::eFifo;
    }

    auto ChooseExtent(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, uint32_t width, uint32_t height) -> vk::Extent2D
    {
        auto capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);

        if (capabilities.currentExtent.width != UINT32_MAX)
        {
            return capabilities.currentExtent;
        }

        vk::Extent2D actualExtent = { width, height };

        // Use Min/Max to clamp the values between the allowed minimum and
        // maximum extents that are supported
        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }

    auto FindDepthFormat(vk::PhysicalDevice physicalDevice) -> vk::Format
    {
        // Since all depth formats may be optional, we need to find a suitable
        // depth format to use. Start with the highest precision packed format.
        std::vector<vk::Format> depthFormats = {
            vk::Format::eD32SfloatS8Uint, vk::Format::eD32Sfloat, vk::Format::eD24UnormS8Uint, vk::Format::eD16UnormS8Uint, vk::Format::eD16Unorm
        };

        for (auto& format : depthFormats)
        {
            vk::FormatProperties formatProps = physicalDevice.getFormatProperties(format);

            // Format must support depth stencil attachment for optimal tiling
            if (formatProps.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
            {
                return format;
            }
        }
        return vk::Format::eUndefined;
    }

    auto FindGraphicsQueueFamily(vk::PhysicalDevice physicalDevice) -> uint32_t
    {
        auto queueProperties = physicalDevice.getQueueFamilyProperties();

        // Find dedicated graphics queue family
        for (uint32_t i = 0; i < queueProperties.size(); i++)
        {
            auto& family = queueProperties[i];
            if ((family.queueFlags & vk::QueueFlagBits::eGraphics) && !(family.queueFlags & vk::QueueFlagBits::eCompute))
            {
                return i;
            }
        }

        // Find any graphics queue family
        for (uint32_t i = 0; i < queueProperties.size(); i++)
        {
            auto& family = queueProperties[i];
            if ((family.queueFlags & vk::QueueFlagBits::eGraphics))
            {
                return i;
            }
        }

        return VK_QUEUE_FAMILY_IGNORED;
    }

}  // namespace gfx::Vulkan
