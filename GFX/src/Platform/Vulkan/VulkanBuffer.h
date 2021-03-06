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
        VulkanBuffer(BufferUsage usage, size_t size, const void* data = nullptr, bool forceLocalMemory = false);
        ~VulkanBuffer() override;

        auto GetSize() const -> size_t { return m_size; }

        auto GetHandle() const -> vk::Buffer { return m_buffer; }

        void SetData(size_t offset, size_t size, const void* data) override;

        auto GetBufferInfo() const -> vk::DescriptorBufferInfo;

    private:
        bool m_forceLocalMemory = false;
        BufferUsage m_usage;
        size_t m_size;

        vk::Buffer m_buffer;
        VmaAllocation m_allocation;
    };
}
