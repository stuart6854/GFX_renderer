//
// Created by stumi on 07/06/21.
//
#ifdef GFX_API_VULKAN

#include "GFX/Debug.h"
#include "GFX/Config.h"

#include "VulkanCore.h"
#include "VulkanUtils.h"

#include <iostream>
#include <array>

namespace gfxOld::Vulkan
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

        std::array<vk::DescriptorPool, Config::FramesInFlight> m_vkDescriptorPools;
        std::array<uint32_t, Config::FramesInFlight> m_vkDescriptorPoolAllocationCounts;

        vk::DebugUtilsMessengerEXT s_vkDebugCallback;
    } // namespace

    void CreateInstance(vk::ApplicationInfo appInfo)
    {
        std::vector<const char*> extensions = {
            VK_KHR_SURFACE_EXTENSION_NAME,
            "VK_KHR_win32_surface",
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
        };

        vk::InstanceCreateInfo instanceInfo{};
        instanceInfo.setPApplicationInfo(&appInfo);
        instanceInfo.setPEnabledExtensionNames(extensions);

        const std::string validationLayerName = "VK_LAYER_KHRONOS_validation";
        if (IsLayerSupported(validationLayerName))
        {
            const char* layerName = validationLayerName.c_str();
            instanceInfo.setEnabledLayerCount(1);
            instanceInfo.setPEnabledLayerNames(layerName);
            GFX_WARN("Vulkan validation layer enabled.");
        }

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

    void CreateDescriptorPool()
    {
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
        poolInfo.setMaxSets(1000);
        poolInfo.setPoolSizes(poolSizes);

        auto device = GetDevice();
        for (uint32_t i = 0; i < Config::FramesInFlight; i++)
        {
            m_vkDescriptorPools[i] = device.createDescriptorPool(poolInfo);
            m_vkDescriptorPoolAllocationCounts[i] = 0;
        }
    }

    void Shutdown()
    {
        s_vkDevice.waitIdle();

        for (auto& pool : m_vkDescriptorPools)
        {
            s_vkDevice.destroy(pool);
        }
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
    auto GetAllocator() -> VulkanAllocator& { return s_vkAllocator; }

    auto GetGraphicsQueueFamily() -> uint32_t { return s_vkGraphicsQueueFamily; }

    auto GetGraphicsQueue() -> vk::Queue { return s_vkGraphicsQueue; }

    auto GetDepthFormat() -> vk::Format { return s_vkDepthFormat; }

    void ResetDescriptorPool(uint32_t frameIndex)
    {
        GetDevice().resetDescriptorPool(m_vkDescriptorPools[frameIndex]);
        m_vkDescriptorPoolAllocationCounts[frameIndex] = 0;
    }

    auto AllocateDescriptorSet(uint32_t frameIndex, const std::vector<vk::DescriptorSetLayout>& layouts) -> vk::DescriptorSet
    {
        vk::DescriptorSetAllocateInfo allocInfo{};
        allocInfo.setDescriptorSetCount(1);
        allocInfo.setSetLayouts(layouts);
        allocInfo.setDescriptorPool(m_vkDescriptorPools[frameIndex]);

        auto device = GetDevice();
        auto descriptorSet = device.allocateDescriptorSets(allocInfo).at(0);

        m_vkDescriptorPoolAllocationCounts[frameIndex] += allocInfo.descriptorSetCount;
        return descriptorSet;
    }

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
} // namespace gfxOld::Vulkan

#endif
