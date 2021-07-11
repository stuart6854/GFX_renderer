#include "GFX/Resources/CommandBuffer.h"

#include "GFX/Core/GFXCore.h"
#include "GFX/Core/Backend.h"

#include "Platform/Vulkan/VulkanCommandBuffer.h"

namespace gfx
{
    auto CommandBuffer::Create(uint32_t count) -> OwnedPtr<CommandBuffer>
    {
        auto backendType = gfx::GetBackendType();
        switch (backendType)
        {
            case BackendType::eVulkan: return CreateOwned<VulkanCommandBuffer>(count);
            case BackendType::eNone:
            default: break;
        }
        return nullptr;
    }
}
