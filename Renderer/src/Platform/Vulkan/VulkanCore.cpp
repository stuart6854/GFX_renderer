//
// Created by stumi on 07/06/21.
//
#ifdef GFX_API_VULKAN

    #include "GFX/Debug.h"

    #include "VulkanCore.h"
    #include "VulkanUtils.h"

    #include <iostream>

namespace gfx::Vulkan
{
    namespace
    {
        vk::Instance s_vkInstance;
        vk::PhysicalDevice s_vkPhysicalDevice;
        vk::Device s_vkDevice;
        VulkanAllocator s_vkAllocator;

        uint32_t s_vkGraphicsQueueFamily;

        vk::Queue s_vkGraphicsQueue;

        vk::Format s_vkDepthFormat;

        vk::DebugUtilsMessengerEXT s_vkDebugCallback;
    }  // namespace

    void CreateInstance(vk::ApplicationInfo appInfo)
    {
        std::vector<const char*> extensions = {
            VK_KHR_SURFACE_EXTENSION_NAME,
            "VK_KHR_win32_surface",
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
        };

        const char* validationLayerName = "VK_LAYER_KHRONOS_validation";

        vk::InstanceCreateInfo instanceInfo{};
        instanceInfo.setPApplicationInfo(&appInfo);
        instanceInfo.setPEnabledExtensionNames(extensions);
        instanceInfo.setEnabledLayerCount(1);
        instanceInfo.setPEnabledLayerNames(validationLayerName);

        s_vkInstance = vk::createInstance(instanceInfo);

        // Setup debug message callback
        vk::DebugUtilsMessengerCreateInfoEXT debugInfo{};
        debugInfo.setMessageSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
                                     vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning);
        debugInfo.setPfnUserCallback(VkDebugCallback);
        debugInfo.setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                                 vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral);

        vk::DispatchLoaderDynamic dldi(s_vkInstance, vkGetInstanceProcAddr);
        s_vkDebugCallback = s_vkInstance.createDebugUtilsMessengerEXT(debugInfo, nullptr, dldi);
    }

    void PickPhysicalDevice()
    {
        auto possibleGPUs = s_vkInstance.enumeratePhysicalDevices();

        for (const auto& gpu : possibleGPUs)
        {
            auto properties = gpu.getProperties();
            if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
            {
                s_vkPhysicalDevice = gpu;
            }
        }

        if (!s_vkPhysicalDevice)
        {
            s_vkPhysicalDevice = possibleGPUs.at(0);
        }

        s_vkDepthFormat = FindDepthFormat(s_vkPhysicalDevice);
    }

    void CreateDevice()
    {
        std::vector<const char*> extensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        };

        vk::DeviceCreateInfo deviceInfo{};
        deviceInfo.setPEnabledExtensionNames(extensions);

        //        PickQueueFamilies();
        s_vkGraphicsQueueFamily = FindGraphicsQueueFamily(s_vkPhysicalDevice);

        static const float graphicsQueuePriority = 1.0f;
        //        static const float transferQueuePriority = 1.0f;

        std::vector<vk::DeviceQueueCreateInfo> queueInfos(1);
        queueInfos[0].setQueueFamilyIndex(s_vkGraphicsQueueFamily);
        queueInfos[0].setQueueCount(1);
        queueInfos[0].pQueuePriorities = &graphicsQueuePriority;
        //
        //        queueInfos[1].setQueueFamilyIndex(m_transferQueueFamily);
        //        queueInfos[1].setQueueCount(1);
        //        queueInfos[1].pQueuePriorities = &transferQueuePriority;
        //
        deviceInfo.setQueueCreateInfos(queueInfos);

        vk::PhysicalDeviceFeatures features{};

        deviceInfo.setPEnabledFeatures(&features);

        s_vkDevice = s_vkPhysicalDevice.createDevice(deviceInfo);

        s_vkGraphicsQueue = s_vkDevice.getQueue(s_vkGraphicsQueueFamily, 0);
        //        m_transferQueue = m_device.getQueue(m_transferQueueFamily, 0);
    }

    void CreateAllocator() { s_vkAllocator.Init(s_vkInstance, s_vkPhysicalDevice, s_vkDevice); }

    void Shutdown()
    {
        s_vkAllocator.Destroy();
        s_vkDevice.destroy();
        s_vkInstance.destroy(s_vkDebugCallback, nullptr, vk::DispatchLoaderDynamic(s_vkInstance, vkGetInstanceProcAddr));
        s_vkInstance.destroy();

        s_vkDevice = nullptr;
        s_vkPhysicalDevice = nullptr;
        s_vkDebugCallback = nullptr;
        s_vkInstance = nullptr;
    }

    auto GetInstance() -> vk::Instance { return s_vkInstance; }
    auto GetPhysicalDevice() -> vk::PhysicalDevice { return s_vkPhysicalDevice; }
    auto GetDevice() -> vk::Device { return s_vkDevice; }
    auto GetAllocator() -> const VulkanAllocator& { return s_vkAllocator; }

    auto GetGraphicsQueueFamily() -> uint32_t { return s_vkGraphicsQueueFamily; }

    auto GetGraphicsQueue() -> vk::Queue { return s_vkGraphicsQueue; }

    auto GetDepthFormat() -> vk::Format { return s_vkDepthFormat; }

    static VKAPI_ATTR auto VKAPI_CALL VkDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                      VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                      const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                      void* pUserData) -> VkBool32
    {
        if (messageType >= (int)vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation)
        {
            GFX_ERROR("*** VULKAN VALIDATION ***\n{}", pCallbackData->pMessage);
        }
        return VK_FALSE;
    }

}  // namespace gfx::Vulkan

#endif