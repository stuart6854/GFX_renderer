#include "GFX/Resources/Texture.h"

#include "GFX/Core/GFXCore.h"
#include "GFX/Resources/ResourceSet.h"
#include "Platform/Vulkan/VulkanTexture.h"

namespace gfx
{
    auto Texture::Create(const TextureBuilder& builder) -> OwnedPtr<Texture>
    {
        auto backendType = gfx::GetBackendType();
        switch (backendType)
        {
            case BackendType::eVulkan: return CreateOwned<VulkanTexture>(builder);
            case BackendType::eNone:
            default: break;
        }
        return nullptr;
    }

    auto Texture::Create(const TextureImporter& importer) -> OwnedPtr<Texture>
    {
        auto backendType = gfx::GetBackendType();
        switch (backendType)
        {
            case BackendType::eVulkan: return CreateOwned<VulkanTexture>(importer);
            case BackendType::eNone:
            default: break;
        }
        return nullptr;
    }


    auto Texture::Create(const TextureDesc& desc) -> OwnedPtr<Texture>
    {
        auto backendType = gfx::GetBackendType();
        switch (backendType)
        {
            case BackendType::eVulkan: return CreateOwned<VulkanTexture>(desc);
            case BackendType::eNone:
            default: break;
        }
        return nullptr;
    }
}
