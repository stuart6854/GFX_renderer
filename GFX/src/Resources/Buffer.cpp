#include "GFX/Resources/Buffer.h"

#include "GFX/Core/GFXCore.h"
#include "Platform/Vulkan/VulkanBuffer.h"

namespace gfx
{
    auto Buffer::Create() -> OwnedPtr<Buffer>
    {
        auto backendType = gfx::GetBackendType();
        switch (backendType)
        {
            case BackendType::eVulkan: return CreateOwned<VulkanBuffer>();
            case BackendType::eNone:
            default: break;
        }
        return nullptr;
    }
}
