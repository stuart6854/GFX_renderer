#include "GFX/Resources/ResourceSet.h"

#include "GFX/Core/GFXCore.h"
#include "GFX/Core/Backend.h"

#include "Platform/Vulkan/VulkanResourceSet.h"

namespace gfx
{
    auto ResourceSet::Allocate(uint32_t frameIndex,
                               uint32_t set,
                               ResourceSetLayout* setLayout) -> OwnedPtr<ResourceSet>
    {
        auto backendType = gfx::GetBackendType();
        switch (backendType)
        {
            case BackendType::eVulkan: return CreateOwned<VulkanResourceSet>(frameIndex, set, setLayout);
            case BackendType::eNone:
            default: break;
        }
        return nullptr;
    }

    auto ResourceSet::Create(uint32_t set, ResourceSetLayout* setLayout) -> OwnedPtr<ResourceSet>
    {
        auto backendType = gfx::GetBackendType();
        switch (backendType)
        {
            case BackendType::eVulkan: return CreateOwned<VulkanResourceSet>(set, setLayout);
            case BackendType::eNone:
            default: break;
        }
        return nullptr;
    }
}
