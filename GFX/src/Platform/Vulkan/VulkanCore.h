//
// Created by stumi on 07/06/21.
//

#ifndef PERSONAL_RENDERER_VULKANCORE_H
#define PERSONAL_RENDERER_VULKANCORE_H

#include "VulkanAllocator.h"

#include <vulkan/vulkan.hpp>

namespace gfx::Vulkan
{
    void CreateInstance(vk::ApplicationInfo appInfo);
    void PickPhysicalDevice();
    void CreateDevice();
    void CreateAllocator();
    void CreateDescriptorPool();

    void Shutdown();

    auto GetInstance() -> vk::Instance;
    auto GetPhysicalDevice() -> vk::PhysicalDevice;
    auto GetDevice() -> vk::Device;
    auto GetAllocator() -> const VulkanAllocator&;

    auto GetGraphicsQueueFamily() -> uint32_t;

    auto GetGraphicsQueue() -> vk::Queue;

    auto GetDepthFormat() -> vk::Format;

    void ResetDescriptorPool(uint32_t frameIndex);

    auto AllocateDescriptorSet(uint32_t frameIndex, const std::vector<vk::DescriptorSetLayout>& layouts) -> vk::DescriptorSet;

    static VKAPI_ATTR auto VKAPI_CALL VkDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                      VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                      const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                      void* pUserData) -> VkBool32;

}  // namespace gfx::Vulkan

#endif  // PERSONAL_RENDERER_VULKANCORE_H
