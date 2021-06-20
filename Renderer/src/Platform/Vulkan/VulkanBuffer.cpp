//
// Created by stumi on 11/06/21.
//

#ifdef GFX_API_VULKAN

    #include "VulkanBuffer.h"

    #include "VulkanCore.h"

namespace gfx
{
    auto BufferTypeToVulkan(BufferType type) -> vk::BufferUsageFlags
    {
        switch (type)
        {
            case BufferType::eVertex: return vk::BufferUsageFlagBits::eVertexBuffer;
            case BufferType::eIndex: return vk::BufferUsageFlagBits::eIndexBuffer;
            case BufferType::eStaging: return vk::BufferUsageFlagBits::eTransferSrc;
        }
        return {};
    }

    Buffer::Buffer(BufferDesc desc) : m_desc(desc)
    {
        auto allocator = Vulkan::GetAllocator();

        auto usage = BufferTypeToVulkan(desc.Type);
        if (desc.Type != BufferType::eStaging) usage |= vk::BufferUsageFlagBits::eTransferDst;

        vk::BufferCreateInfo bufferInfo{};
        bufferInfo.setSize(desc.Size);
        bufferInfo.setUsage(usage);

        auto memoryType = (desc.Type == BufferType::eStaging) ? VMA_MEMORY_USAGE_CPU_ONLY : VMA_MEMORY_USAGE_GPU_ONLY;

        allocator.Allocate(bufferInfo, memoryType, &m_buffer, &m_allocation);
    }

    Buffer::~Buffer()
    {
        auto allocator = Vulkan::GetAllocator();

        allocator.Free(m_buffer, m_allocation);
    }

    auto Buffer::GetType() const -> BufferType { return m_desc.Type; }

    auto Buffer::GetSize() const -> uint32_t { return m_desc.Size; }

    auto Buffer::GetStride() const -> uint32_t { return m_desc.Stride; }

    auto Buffer::GetNumElements() const -> uint32_t { return m_desc.NumElements; }

    void Buffer::SetAPIResource(vk::Buffer buffer, VmaAllocation allocation)
    {
        m_buffer = buffer;
        m_allocation = allocation;
    }

    auto Buffer::GetAPIBuffer() const -> vk::Buffer { return m_buffer; }

    auto Buffer::GetAPIAllocation() const -> VmaAllocation { return m_allocation; }

}  // namespace gfx

#endif
