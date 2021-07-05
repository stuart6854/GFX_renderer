//
// Created by stumi on 10/06/21.
//

#include "VulkanAllocator.h"

#include "GFX/Debug.h"

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

namespace gfxOld
{
    void VulkanAllocator::Init(vk::Instance instance, vk::PhysicalDevice physicalDevice, vk::Device device)
    {
        VmaAllocatorCreateInfo createInfo{};
        createInfo.vulkanApiVersion = VK_API_VERSION_1_2;
        createInfo.instance = instance;
        createInfo.physicalDevice = physicalDevice;
        createInfo.device = device;

        vmaCreateAllocator(&createInfo, &m_allocator);
    }

    void VulkanAllocator::Destroy()
    {
        for (auto& [buffer, alloc] : m_bufferAllocs)
        {
            vmaDestroyBuffer(m_allocator, buffer, alloc);
        }
        m_bufferAllocs.clear();
        for (auto& [image, alloc] : m_imageAllocs)
        {
            vmaDestroyImage(m_allocator, image, alloc);
        }
        m_imageAllocs.clear();

        vmaDestroyAllocator(m_allocator);
    }

    void VulkanAllocator::Allocate(vk::ImageCreateInfo imageInfo, VmaMemoryUsage memoryUsage, vk::Image* image, VmaAllocation* allocation)
    {
        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = memoryUsage;

        VkImageCreateInfo rawImageInfo = imageInfo;

        VkImage rawImage{};
        vmaCreateImage(m_allocator, &rawImageInfo, &allocInfo, &rawImage, allocation, nullptr);

        *image = rawImage;

        m_imageAllocs[*image] = *allocation;
    }

    void VulkanAllocator::Allocate(vk::BufferCreateInfo imageInfo, VmaMemoryUsage memoryUsage, vk::Buffer* buffer, VmaAllocation* allocation)
    {
        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = memoryUsage;

        VkBufferCreateInfo rawBufferInfo = imageInfo;

        VkBuffer rawBuffer{};
        vmaCreateBuffer(m_allocator, &rawBufferInfo, &allocInfo, &rawBuffer, allocation, nullptr);

        *buffer = rawBuffer;

        m_bufferAllocs[*buffer] = *allocation;
    }

    void VulkanAllocator::Free(vk::Image& image, VmaAllocation& allocation)
    {
        if (!image) return;

        vmaDestroyImage(m_allocator, image, allocation);

        m_imageAllocs.erase(image);
    }

    void VulkanAllocator::Free(vk::Buffer& buffer, VmaAllocation& allocation)
    {
        if (!buffer) return;

        vmaDestroyBuffer(m_allocator, buffer, allocation);

        m_bufferAllocs.erase(buffer);
    }

    auto VulkanAllocator::Map(VmaAllocation allocation) -> void*
    {
        void* mapped = nullptr;
        vmaMapMemory(m_allocator, allocation, &mapped);
        return mapped;
    }

    void VulkanAllocator::Unmap(VmaAllocation allocation) { vmaUnmapMemory(m_allocator, allocation); }
} // namespace gfxOld
