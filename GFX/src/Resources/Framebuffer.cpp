#include "GFX/Resources/Framebuffer.h"

#include "GFX/Core/GFXCore.h"
#include "GFX/Core/Backend.h"

#include "Platform/Vulkan/VulkanFramebuffer.h"

namespace gfx
{
    auto Framebuffer::Create(SwapChain* swapChain) -> OwnedPtr<Framebuffer>
    {
        auto backendType = gfx::GetBackendType();
        switch (backendType)
        {
            case BackendType::eVulkan: return CreateOwned<VulkanFramebuffer>(swapChain);
            case BackendType::eNone:
            default: break;
        }
        return nullptr;
    }

    auto Framebuffer::Create(const FramebufferDesc& desc) -> OwnedPtr<Framebuffer>
    {
        auto backendType = gfx::GetBackendType();
        switch (backendType)
        {
            case BackendType::eVulkan: return CreateOwned<VulkanFramebuffer>(desc);
            case BackendType::eNone:
            default: break;
        }
        return nullptr;
    }
}
