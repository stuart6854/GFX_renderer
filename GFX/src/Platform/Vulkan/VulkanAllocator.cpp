#include "VulkanAllocator.h"

#include "GFX/Debug.h"

#pragma warning(push)
#pragma warning(disable : 4100)
#pragma warning(disable : 4127)
#pragma warning(disable : 4324)
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"
#pragma warning(pop)

namespace gfx
{
    VulkanAllocator::VulkanAllocator(vk::Instance instance, vk::PhysicalDevice physicalDevice, vk::Device device)
    {
        VmaAllocatorCreateInfo createInfo{};
        createInfo.vulkanApiVersion = VK_API_VERSION_1_2;
        createInfo.instance = instance;
        createInfo.physicalDevice = physicalDevice;
        createInfo.device = device;

        vmaCreateAllocator(&createInfo, &m_allocator);
    }

    VulkanAllocator::~VulkanAllocator()
    {
        vmaDestroyAllocator(m_allocator);
    }

    void VulkanAllocator::Allocate(vk::ImageCreateInfo imageInfo, VmaMemoryUsage memoryUsage, vk::Image* image, VmaAllocation* allocation)
    {
        {
            VmaAllocationCreateInfo allocInfo{};
            allocInfo.usage = memoryUsage;

            VkImageCreateInfo rawImageInfo = imageInfo;

            VkImage rawImage{};
            vmaCreateImage(m_allocator, &rawImageInfo, &allocInfo, &rawImage, allocation, nullptr);
            *image = rawImage;
        }
        {
            VmaAllocationInfo allocInfo;
            vmaGetAllocationInfo(m_allocator, *allocation, &allocInfo);

            GFX_TRACE("VulkanAllocator: Image allocated. Size = {}", allocInfo.size);
        }
    }

    void VulkanAllocator::Allocate(vk::BufferCreateInfo imageInfo, VmaMemoryUsage memoryUsage, vk::Buffer* buffer, VmaAllocation* allocation)
    {
        {
            VmaAllocationCreateInfo allocInfo{};
            allocInfo.usage = memoryUsage;

            VkBufferCreateInfo rawBufferInfo = imageInfo;

            VkBuffer rawBuffer{};
            vmaCreateBuffer(m_allocator, &rawBufferInfo, &allocInfo, &rawBuffer, allocation, nullptr);
            *buffer = rawBuffer;
        }
        {
            VmaAllocationInfo allocInfo;
            vmaGetAllocationInfo(m_allocator, *allocation, &allocInfo);

            GFX_TRACE("VulkanAllocator: Buffer allocated. Size = {}", allocInfo.size);
        }
    }

    void VulkanAllocator::Free(vk::Image& image, VmaAllocation& allocation)
    {
        if (!image) return;

        VmaAllocationInfo allocInfo;
        vmaGetAllocationInfo(m_allocator, allocation, &allocInfo);

        vmaDestroyImage(m_allocator, image, allocation);

        GFX_TRACE("VulkanAllocator: Image freed. Size = {}", allocInfo.size);
    }

    void VulkanAllocator::Free(vk::Buffer& buffer, VmaAllocation& allocation)
    {
        if (!buffer) return;

        VmaAllocationInfo allocInfo;
        vmaGetAllocationInfo(m_allocator, allocation, &allocInfo);

        vmaDestroyBuffer(m_allocator, buffer, allocation);

        GFX_TRACE("VulkanAllocator: Buffer freed. Size = {}", allocInfo.size);
    }

    auto VulkanAllocator::Map(VmaAllocation allocation) -> void*
    {
        void* mapped = nullptr;
        vmaMapMemory(m_allocator, allocation, &mapped);
        return mapped;
    }

    void VulkanAllocator::Unmap(VmaAllocation allocation) { vmaUnmapMemory(m_allocator, allocation); }
}
