#include "GFX/Resources/Shader.h"

#include "GFX/Core/GFXCore.h"
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
}
