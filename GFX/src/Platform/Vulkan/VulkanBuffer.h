#pragma once

#include "GFX/Resources/Buffer.h"
#include "vk_mem_alloc.h"

#include <vulkan/vulkan.hpp>

#include <cstdint>

namespace gfx
{
    class VulkanBuffer : public Buffer
    {
    public:
        VulkanBuffer(BufferUsage usage, uint64_t size, const void* data = nullptr);
        ~VulkanBuffer() override;

        auto GetSize() const -> uint64_t { return m_size; }

        auto GetHandle() const -> vk::Buffer { return m_buffer; }

        void SetData(uint32_t offset, uint32_t size, const void* data) override;

        auto GetBufferInfo() const -> vk::DescriptorBufferInfo;

    private:
        BufferUsage m_usage;
        uint64_t m_size;

        vk::Buffer m_buffer;
        VmaAllocation m_allocation;
    };
}
