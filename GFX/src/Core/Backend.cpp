#include "GFX/Core/Backend.h"

#include "Platform/Vulkan/VulkanBackend.h"

namespace gfx
{
    auto Backend::Create(BackendType type,
                         bool enableDebugLayer) -> OwnedPtr<Backend>
    {
        switch (type)
        {
            case BackendType::eVulkan: return CreateOwned<VulkanBackend>(enableDebugLayer);
            case BackendType::eNone:
            default: break;
        }
        return nullptr;
    }
}
