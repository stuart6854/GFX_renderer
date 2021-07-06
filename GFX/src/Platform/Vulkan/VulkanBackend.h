#pragma once

#include "GFX/Core/Base.h"
#include "GFX/Core/GFXCore.h"
#include "GFX/Core/Backend.h"

#include "VulkanDevice.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanAllocator.h"

#include <vulkan/vulkan.hpp>

namespace gfx
{
    class VulkanBackend : public Backend
    {
    public:
        static auto Get() -> VulkanBackend* { return static_cast<VulkanBackend*>(GetBackend()); }

        VulkanBackend(bool enableDebugLayer);
        ~VulkanBackend();

        auto GetInstance() -> vk::Instance& { return m_instance; }
        auto GetPhysicalDevice() -> VulkanPhysicalDevice& { return *m_physicalDevice; }
        auto GetDevice() -> VulkanDevice& { return *m_device; }
        auto GetAllocator() -> VulkanAllocator& { return *m_allocator; }

        void WaitIdle() override;

    private:
        void CreateInstance();
        void PickPhysicalDevice();
        void CreateDevice();
        void CreateAllocator();

    private:
        vk::Instance m_instance;
        vk::DebugUtilsMessengerEXT m_debugCallback;

        OwnedPtr<VulkanPhysicalDevice> m_physicalDevice;
        OwnedPtr<VulkanDevice> m_device;
        OwnedPtr<VulkanAllocator> m_allocator;
    };
}
