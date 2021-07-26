#include "GFX/Resources/Shader.h"

#include "GFX/Core/GFXCore.h"
#include "GFX/Resources/ResourceSet.h"
#include "Platform/Vulkan/VulkanShader.h"

namespace gfx
{
    auto Shader::Create(const std::string& vertexSource, const std::string& pixelSource) -> OwnedPtr<Shader>
    {
        auto backendType = gfx::GetBackendType();
        switch (backendType)
        {
            case BackendType::eVulkan: return CreateOwned<VulkanShader>(vertexSource, pixelSource);
            case BackendType::eNone:
            default: break;
        }
        return nullptr;
    }

    auto Shader::CreateResourceSet(uint32_t frameIndex,
                                   uint32_t set) -> OwnedPtr<ResourceSet>
    {
        return ResourceSet::Create(frameIndex, set, m_resourceSetLayouts[set].get());
    }
}
