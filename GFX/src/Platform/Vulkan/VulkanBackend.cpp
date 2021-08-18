#include "VulkanBackend.h"

#include "GFX/Debug.h"

#include <string>
#include <vector>
#include <iostream>

namespace gfx
{
    namespace Utils
    {
        bool IsLayerSupported(const std::string& layerName)
        {
            const auto layerProperties = vk::enumerateInstanceLayerProperties();

            for (const vk::LayerProperties& layerProps : layerProperties)
            {
                const std::string vkLayerName = layerProps.layerName.data();
                if (vkLayerName == layerName) return true;
            }

            return false;
        }

        static VKAPI_ATTR auto VKAPI_CALL VkDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT,
                                                          VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                          const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                          void*) -> VkBool32
        {
            if (messageType >= (int)vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation)
            {
                GFX_ERROR("*** VULKAN VALIDATION ***\n{}", pCallbackData->pMessage);
            }
            return VK_FALSE;
        }
    }  // namespace Utils

    VulkanBackend::VulkanBackend(bool enableDebugLayer)
    {
        CreateInstance(enableDebugLayer);
        PickPhysicalDevice();
        CreateDevice();
        CreateAllocator();
    }

    VulkanBackend::~VulkanBackend() {}

    void VulkanBackend::WaitIdle() { m_device->WaitIdle(); }

    void VulkanBackend::CreateInstance(bool enableDebugLayer)
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

        std::vector<const char*> layers = {};

        vk::InstanceCreateInfo instanceInfo{};
        instanceInfo.setPApplicationInfo(&appInfo);
        instanceInfo.setPEnabledExtensionNames(extensions);

        if (enableDebugLayer)
        {
            const std::string validationLayerName = "VK_LAYER_KHRONOS_validation";
            if (Utils::IsLayerSupported(validationLayerName))
            {
                layers.push_back(validationLayerName.c_str());
                GFX_INFO("Vulkan validation layer enabled.");
            }
            else
            {
                GFX_WARN("Vulkan validation layer not supported!");
            }
        }

        instanceInfo.setPEnabledLayerNames(layers);

        m_instance = vk::createInstance(instanceInfo);
        GFX_ASSERT(m_instance, "vk::createInstance() failed!");

        if (enableDebugLayer)
        {
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
    }

    void VulkanBackend::PickPhysicalDevice() { m_physicalDevice = VulkanPhysicalDevice::Select(m_instance); }

    void VulkanBackend::CreateDevice() { m_device = CreateOwned<VulkanDevice>(*m_physicalDevice); }

    void VulkanBackend::CreateAllocator() { m_allocator = CreateOwned<VulkanAllocator>(m_instance, m_physicalDevice->GetHandle(), m_device->GetHandle()); }

}  // namespace gfx
