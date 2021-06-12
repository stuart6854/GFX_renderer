//
// Created by stumi on 10/06/21.
//

#ifndef PERSONAL_RENDERER_VULKANALLOCATOR_H
#define PERSONAL_RENDERER_VULKANALLOCATOR_H

#include "vk_mem_alloc.h"

#include <vulkan/vulkan.hpp>

namespace gfx
{
    class VulkanAllocator
    {
    public:
        VulkanAllocator() = default;
        ~VulkanAllocator() = default;

        void Init(vk::Instance instance, vk::PhysicalDevice physicalDevice, vk::Device device);
        void Destroy();

        void Allocate(vk::ImageCreateInfo imageInfo, VmaMemoryUsage memoryUsage, vk::Image* image, VmaAllocation* allocation) const;
        void Allocate(vk::BufferCreateInfo imageInfo, VmaMemoryUsage memoryUsage, vk::Buffer* buffer, VmaAllocation* allocation) const;

        void Free(vk::Image& image, VmaAllocation& allocation) const;
        void Free(vk::Buffer& buffer, VmaAllocation& allocation) const;

        auto Map(VmaAllocation allocation) -> void*;
        void Unmap(VmaAllocation allocation);

    private:
        VmaAllocator m_allocator;
    };

}  // namespace gfx

#endif  // PERSONAL_RENDERER_VULKANALLOCATOR_H
