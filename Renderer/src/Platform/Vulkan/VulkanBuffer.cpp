//
// Created by stumi on 11/06/21.
//

#ifdef GFX_API_VULKAN

    #include "VulkanBuffer.h"

namespace gfx
{
    auto BufferTypeToVulkan(BufferType type) -> vk::BufferUsageFlags
    {
        switch (type)
        {
            case eVertex:
                return vk::BufferUsageFlagBits::eVertexBuffer;
            case eIndex:
                return vk::BufferUsageFlagBits::eIndexBuffer;
            case eTransfer:
                vk::BufferUsageFlagBits::eTransferSrc;
        }
        return {};
    }

    Buffer::Buffer(BufferDesc desc) : m_desc(desc) {}

    auto Buffer::GetType() const -> BufferType { return m_desc.Type; }

    auto Buffer::GetSize() const -> uint32_t { return m_desc.Size; }

    auto Buffer::GetStride() const -> uint32_t { return m_desc.Stride; }

    auto Buffer::GetNumElements() const -> uint32_t { return m_desc.NumElements; }

    void Buffer::SetAPIResource(vk::Buffer buffer, VmaAllocation allocation)
    {
        m_buffer = buffer;
        m_allocation = allocation;
    }

    auto Buffer::GetAPIResource() const -> vk::Buffer { return m_buffer; }

}  // namespace gfx

#endif
