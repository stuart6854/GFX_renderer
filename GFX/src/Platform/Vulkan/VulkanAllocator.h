#pragma once

#pragma warning(push)
#pragma warning(disable : 4100)
#pragma warning(disable : 4127)
#pragma warning(disable : 4324)
#include "vk_mem_alloc.h"
#pragma warning(pop)

#include <vulkan/vulkan.hpp>

namespace gfx
{
    class VulkanAllocator
    {
    public:
        VulkanAllocator(vk::Instance instance, vk::PhysicalDevice physicalDevice, vk::Device device);
        ~VulkanAllocator();

        void Allocate(vk::ImageCreateInfo imageInfo, VmaMemoryUsage memoryUsage, vk::Image* image, VmaAllocation* allocation);
        void Allocate(vk::BufferCreateInfo imageInfo, VmaMemoryUsage memoryUsage, vk::Buffer* buffer, VmaAllocation* allocation);

        void Free(vk::Image& image, VmaAllocation& allocation);
        void Free(vk::Buffer& buffer, VmaAllocation& allocation);

        auto Map(VmaAllocation allocation) -> void*;
        void Unmap(VmaAllocation allocation);

    private:
        VmaAllocator m_allocator;

    };
}
