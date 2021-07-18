#include "GFX/Resources/ResourceSetLayout.h"

#include "GFX/Core/GFXCore.h"
#include "GFX/Core/Backend.h"

#include "Platform/Vulkan/VulkanResourceSetLayout.h"

namespace gfx
{
    auto ResourceSetLayout::Create() -> OwnedPtr<ResourceSetLayout>
    {
        auto backendType = gfx::GetBackendType();
        switch (backendType)
        {
            case BackendType::eVulkan: return CreateOwned<VulkanResourceSetLayout>();
            case BackendType::eNone:
            default: break;
        }
        return nullptr;
    }
}
