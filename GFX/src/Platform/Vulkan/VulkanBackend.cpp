#include "VulkanBackend.h"

#include <string>
#include <vector>
#include <iostream>

namespace gfx
{
    namespace Utils
    {
        bool IsLayerSupported(const std::string& layerName)
        {
            auto layers = vk::enumerateInstanceLayerProperties();
            for (const auto& layer : layers)
            {
                if (layer.layerName == layerName)
                    return true;
            }

            return false;
        }

        static VKAPI_ATTR auto VKAPI_CALL VkDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                          VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                          const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                          void* pUserData) -> VkBool32
        {
            if (messageType >= (int)vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation)
            {
                // GFX_ERROR("*** VULKAN VALIDATION ***\n{}", pCallbackData->pMessage);
                std::cout << "*** VULKAN VALIDATION ***\n" << pCallbackData->pMessage << std::endl;
            }
            return VK_FALSE;
        }
    }

    VulkanBackend::VulkanBackend(bool enableDebugLayer)
        : Backend(enableDebugLayer)
    {
        CreateInstance();
        PickPhysicalDevice();
        CreateDevice();
        CreateAllocator();
    }

    VulkanBackend::~VulkanBackend()
    {
    }

    void VulkanBackend::WaitIdle()
    {
        m_device->WaitIdle();
    }

    void VulkanBackend::CreateInstance()
    {
        vk::ApplicationInfo appInfo{};
        appInfo.setApiVersion(VK_API_VERSION_1_2);
        appInfo.setPEngineName("GFX");
        appInfo.setPApplicationName("GFX");

        std::vector<const char*> extensions = {
            VK_KHR_SURFACE_EXTENSION_NAME,
            "VK_KHR_win32_surface",
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
        };

        vk::InstanceCreateInfo instanceInfo{};
        instanceInfo.setPApplicationInfo(&appInfo);
        instanceInfo.setPEnabledExtensionNames(extensions);

        const std::string validationLayerName = "VK_LAYER_KHRONOS_validation";
        if (Utils::IsLayerSupported(validationLayerName))
        {
            const char* layerName = validationLayerName.c_str();
            instanceInfo.setEnabledLayerCount(1);
            instanceInfo.setPEnabledLayerNames(layerName);
            // GFX_WARN("Vulkan validation layer enabled.");
        }

        m_instance = vk::createInstance(instanceInfo);

        // Setup debug message callback
        vk::DebugUtilsMessengerCreateInfoEXT debugInfo{};
        debugInfo.setMessageSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning);
        debugInfo.setPfnUserCallback(Utils::VkDebugCallback);
        debugInfo.setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral);

        vk::DispatchLoaderDynamic dldi(m_instance, vkGetInstanceProcAddr);
        m_debugCallback = m_instance.createDebugUtilsMessengerEXT(debugInfo, nullptr, dldi);
    }

    void VulkanBackend::PickPhysicalDevice()
    {
        m_physicalDevice = VulkanPhysicalDevice::Select(m_instance);
    }

    void VulkanBackend::CreateDevice()
    {
        m_device = CreateOwned<VulkanDevice>(*m_physicalDevice);
    }

    void VulkanBackend::CreateAllocator()
    {
        m_allocator = CreateOwned<VulkanAllocator>(m_instance, m_physicalDevice->GetHandle(), m_device->GetHandle());
    }
}
