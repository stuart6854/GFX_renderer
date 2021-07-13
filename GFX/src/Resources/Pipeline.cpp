#include "GFX/Resources/Pipeline.h"

#include "GFX/Core/GFXCore.h"
#include "Platform/Vulkan/VulkanPipeline.h"

namespace gfx
{
    auto Pipeline::Create(const PipelineDesc& desc) -> OwnedPtr<Pipeline>
    {
        auto backendType = gfx::GetBackendType();
        switch (backendType)
        {
            case BackendType::eVulkan: return CreateOwned<VulkanPipeline>(desc);
            case BackendType::eNone:
            default: break;
        }
        return nullptr;
    }
}
