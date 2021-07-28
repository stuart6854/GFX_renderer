#include "GFX/Resources/Buffer.h"

#include "GFX/Core/GFXCore.h"
#include "Platform/Vulkan/VulkanBuffer.h"

namespace gfx
{
    auto Buffer::Create(BufferUsage usage, uint64_t size, const void* data, bool forceLocalMemory) -> OwnedPtr<Buffer>
    {
        auto backendType = gfx::GetBackendType();
        switch (backendType)
        {
            case BackendType::eVulkan: return CreateOwned<VulkanBuffer>(usage, size, data, forceLocalMemory);
            case BackendType::eNone:
            default: break;
        }
        return nullptr;
    }

    auto Buffer::CreateStaging(uint64_t size, const void* data) -> OwnedPtr<Buffer>
    {
        return Create(BufferUsage::eStaging, size, data);
    }

    auto Buffer::CreateVertex(uint64_t size, const void* data, bool forceLocalMemory) -> OwnedPtr<Buffer>
    {
        return Create(BufferUsage::eVertex, size, data, forceLocalMemory);
    }

    auto Buffer::CreateIndex(uint64_t size, const void* data, bool forceLocalMemory) -> OwnedPtr<Buffer>
    {
        return Create(BufferUsage::eIndex, size, data, forceLocalMemory);
    }

    auto Buffer::CreateUniform(uint64_t size, const void* data) -> OwnedPtr<Buffer>
    {
        return Create(BufferUsage::eUniform, size, data);
    }
}
