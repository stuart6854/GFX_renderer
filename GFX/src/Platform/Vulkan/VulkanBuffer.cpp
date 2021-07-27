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

        auto ToMemUsage(BufferUsage usage) -> VmaMemoryUsage
        {
            switch (usage)
            {
                default:
                case BufferUsage::eNone: break;
                case BufferUsage::eStaging: return VMA_MEMORY_USAGE_CPU_ONLY;
                case BufferUsage::eVertex:
                case BufferUsage::eIndex: return VMA_MEMORY_USAGE_GPU_ONLY;
                case BufferUsage::eUniform: return VMA_MEMORY_USAGE_CPU_TO_GPU;
            }
            return {};
        }
    }

    VulkanBuffer::VulkanBuffer(BufferUsage usage, uint64_t size, const void* data)
        : m_usage(usage),
          m_size(size)
    {
        auto* backend = VulkanBackend::Get();
        auto& allocator = backend->GetAllocator();

        auto vkUsage = Utils::ToVkBufferUsage(usage);
        if (usage != BufferUsage::eStaging) vkUsage |= vk::BufferUsageFlagBits::eTransferDst;

        auto memUsage = Utils::ToMemUsage(usage);

        vk::BufferCreateInfo bufferInfo{};
        bufferInfo.setSize(m_size);
        bufferInfo.setUsage(vkUsage);

        allocator.Allocate(bufferInfo, memUsage, &m_buffer, &m_allocation);

        if (data != nullptr)
        {
            SetData(0, size, data);
        }
    }

    VulkanBuffer::~VulkanBuffer()
    {
        auto* backend = VulkanBackend::Get();
        auto& allocator = backend->GetAllocator();

        allocator.Free(m_buffer, m_allocation);
    }

    void VulkanBuffer::SetData(uint32_t offset, const uint32_t size, const void* data)
    {
        auto* backend = VulkanBackend::Get();
        auto& allocator = backend->GetAllocator();

        if (m_usage == BufferUsage::eStaging || m_usage == BufferUsage::eUniform)
        {
            // Direct copy
            auto* mapped = allocator.Map(m_allocation);
            std::memcpy(mapped, data, size);
            allocator.Unmap(m_allocation);
        }
        else
        {
            // Staging buffer
            const auto stagingBuffer = CreateStaging(size, data);
            auto* vkStagingBuffer = static_cast<VulkanBuffer*>(stagingBuffer.get());

            vk::BufferCopy copyRegion{};
            copyRegion.setSize(size);

            auto& device = backend->GetDevice();
            const auto cmdBuffer = device.GetCommandBuffer(true);
            cmdBuffer.copyBuffer(vkStagingBuffer->GetHandle(), m_buffer, copyRegion);
            device.FlushCommandBuffer(cmdBuffer);
        }
    }

    auto VulkanBuffer::GetBufferInfo() const -> vk::DescriptorBufferInfo
    {
        vk::DescriptorBufferInfo info{};
        info.setBuffer(m_buffer);
        info.setOffset(0);
        info.setRange(m_size);
        return info;
    }
}
