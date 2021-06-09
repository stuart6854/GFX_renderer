//
// Created by stumi on 07/06/21.
//

#ifndef PERSONAL_RENDERER_VULKANCORE_H
#define PERSONAL_RENDERER_VULKANCORE_H

#include <vulkan/vulkan.hpp>

namespace gfx::Vulkan
{
    void CreateInstance(vk::ApplicationInfo appInfo);
    void PickPhysicalDevice();
    void CreateDevice();

    void Shutdown();

    auto GetInstance() -> vk::Instance;
    auto GetPhysicalDevice() -> vk::PhysicalDevice;
    auto GetDevice() -> vk::Device;

    auto GetGraphicsQueueFamily() -> uint32_t;

    auto GetGraphicsQueue() -> vk::Queue;

    static VKAPI_ATTR auto VKAPI_CALL VkDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                      VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                      const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                      void* pUserData) -> VkBool32;

}  // namespace gfx::Vulkan

#endif  // PERSONAL_RENDERER_VULKANCORE_H
