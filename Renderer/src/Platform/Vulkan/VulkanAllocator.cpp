//
// Created by stumi on 10/06/21.
//

#include "VulkanAllocator.h"

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

namespace gfx
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

    void VulkanAllocator::Destroy() { vmaDestroyAllocator(m_allocator); }

    void VulkanAllocator::Allocate(vk::ImageCreateInfo imageInfo, VmaMemoryUsage memoryUsage, vk::Image* image, VmaAllocation* allocation) const
    {
        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = memoryUsage;

        VkImageCreateInfo rawImageInfo = imageInfo;

        VkImage rawImage{};
        vmaCreateImage(m_allocator, &rawImageInfo, &allocInfo, &rawImage, allocation, nullptr);

        *image = rawImage;
    }

    void VulkanAllocator::Allocate(vk::BufferCreateInfo imageInfo, VmaMemoryUsage memoryUsage, vk::Buffer* buffer, VmaAllocation* allocation) const
    {
        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = memoryUsage;

        VkBufferCreateInfo rawBufferInfo = imageInfo;

        VkBuffer rawBuffer{};
        vmaCreateBuffer(m_allocator, &rawBufferInfo, &allocInfo, &rawBuffer, allocation, nullptr);

        *buffer = rawBuffer;
    }

    void VulkanAllocator::Free(vk::Image& image, VmaAllocation& allocation) const
    {
        if (!image) return;

        vmaDestroyImage(m_allocator, image, allocation);
    }

    void VulkanAllocator::Free(vk::Buffer& buffer, VmaAllocation& allocation) const
    {
        if (!buffer) return;

        vmaDestroyBuffer(m_allocator, buffer, allocation);
    }

}  // namespace gfx