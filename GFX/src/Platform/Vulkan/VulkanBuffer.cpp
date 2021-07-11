#include "VulkanBuffer.h"

#include "VulkanBackend.h"
#include "VulkanAllocator.h"

namespace gfx
{
    namespace Utils
    {
        auto ToVkBufferUsage(BufferUsage usage) -> vk::BufferUsageFlags
        {
            switch (usage)
            {
                default:
                case BufferUsage::eNone: break;
                case BufferUsage::eStaging: return vk::BufferUsageFlagBits::eTransferSrc;
                case BufferUsage::eVertex: return vk::BufferUsageFlagBits::eVertexBuffer;
                case BufferUsage::eIndex: return vk::BufferUsageFlagBits::eIndexBuffer;
                case BufferUsage::eUniform: return vk::BufferUsageFlagBits::eUniformBuffer;
            }
            return {};
        }
    }

    VulkanBuffer::VulkanBuffer(BufferUsage usage, uint64_t size, const void* data)
        : m_size(size)
    {
        auto* backend = VulkanBackend::Get();
        auto& allocator = backend->GetAllocator();

        auto vkUsage = Utils::ToVkBufferUsage(usage);
        if (usage != BufferUsage::eStaging) vkUsage |= vk::BufferUsageFlagBits::eTransferDst;

        auto memUsage = (usage == BufferUsage::eStaging ? VMA_MEMORY_USAGE_CPU_ONLY : VMA_MEMORY_USAGE_GPU_ONLY);

        vk::BufferCreateInfo bufferInfo{};
        bufferInfo.setSize(m_size);
        bufferInfo.setUsage(vkUsage);

        allocator.Allocate(bufferInfo, memUsage, &m_buffer, &m_allocation);

        if (data != nullptr)
        {
            if (usage == BufferUsage::eStaging)
            {
                // Direct copy
                auto* mapped = allocator.Map(m_allocation);
                std::memcpy(mapped, data, m_size);
                allocator.Unmap(m_allocation);
            }
            else
            {
                // Staging buffer
                auto stagingBuffer = Buffer::CreateStaging(m_size, data);
                auto* vkStagingBuffer = static_cast<VulkanBuffer*>(stagingBuffer.get());

                vk::BufferCopy copyRegion{};
                copyRegion.setSize(size);

                auto& device = backend->GetDevice();
                auto cmdBuffer = device.GetCommandBuffer(true);
                cmdBuffer.copyBuffer(vkStagingBuffer->GetHandle(), m_buffer, copyRegion);
                device.FlushCommandBuffer(cmdBuffer);
            }
        }
    }

    VulkanBuffer::~VulkanBuffer()
    {
        auto* backend = VulkanBackend::Get();
        auto& allocator = backend->GetAllocator();

        allocator.Free(m_buffer, m_allocation);
    }
}
