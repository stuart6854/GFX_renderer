//
// Created by stumi on 20/06/21.
//

#ifdef GFX_API_VULKAN

    #include "VulkanUniformBuffer.h"

    #include "VulkanCore.h"
    #include "VulkanAllocator.h"

namespace gfx
{
    UniformBuffer::UniformBuffer(uint32_t size, uint32_t binding) : m_size(size), m_binding(binding)
    {
        BufferDesc bufferDesc = { .Type = BufferType::eUniformBuffer, .Size = size };
        m_buffer = std::make_shared<Buffer>(bufferDesc);
    }

    void UniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
    {
        auto allocator = Vulkan::GetAllocator();

        auto mapped = (uint8_t*)allocator.Map(m_buffer->GetAPIAllocation());
        std::memcpy(mapped + offset, data, size);
        allocator.Unmap(m_buffer->GetAPIAllocation());
    }

}  // namespace gfx

#endif