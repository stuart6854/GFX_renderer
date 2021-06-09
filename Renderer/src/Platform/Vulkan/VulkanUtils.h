//
// Created by stumi on 07/06/21.
//

#ifndef PERSONAL_RENDERER_VULKANUTILS_H
#define PERSONAL_RENDERER_VULKANUTILS_H

#include <vulkan/vulkan.hpp>

namespace gfx::Vulkan
{
    auto ChooseSurfaceFormat(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface) -> vk::SurfaceFormatKHR;

    auto ChoosePresentMode(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface) -> vk::PresentModeKHR;

    auto ChooseExtent(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, uint32_t width, uint32_t height) -> vk::Extent2D;

    auto FindGraphicsQueueFamily(vk::PhysicalDevice physicalDevice) -> uint32_t;

}  // namespace gfx::Vulkan

#endif  // PERSONAL_RENDERER_VULKANUTILS_H
