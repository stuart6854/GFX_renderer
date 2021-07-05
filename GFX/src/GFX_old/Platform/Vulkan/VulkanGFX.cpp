//
// Created by stumi on 07/06/21.
//

#ifdef GFX_API_VULKAN

    #include "GFX/GFX.h"
    #include "GFX/Debug.h"

    #include "VulkanCore.h"

    #include <vulkan/vulkan.hpp>

namespace gfxOld
{
    void Init()
    {
        GFX_INFO("Initialising Vulkan Renderer!");

        auto instanceVersion = vk::enumerateInstanceVersion();
        uint32_t major = VK_VERSION_MAJOR(instanceVersion);
        uint32_t minor = VK_VERSION_MINOR(instanceVersion);
        uint32_t patch = VK_VERSION_PATCH(instanceVersion);
        GFX_INFO("Vulkan version: {}.{}.{}", major, minor, patch);

        vk::ApplicationInfo appInfo{};
        appInfo.apiVersion = VK_API_VERSION_1_2;

        Vulkan::CreateInstance(appInfo);
        Vulkan::PickPhysicalDevice();
        Vulkan::CreateDevice();
        Vulkan::CreateAllocator();
        Vulkan::CreateDescriptorPool();

        auto gpu = Vulkan::GetPhysicalDevice().getProperties();
        GFX_INFO("Graphics Card: {}", gpu.deviceName);
    }

    void Shutdown()
    {
        GFX_INFO("Shutting down Vulkan Renderer!");

        Vulkan::Shutdown();
    }

}  // namespace gfxOld

#endif