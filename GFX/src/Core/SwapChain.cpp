#include "GFX/Core/SwapChain.h"

#include "GFX/Core/GFXCore.h"
#include "Platform/Vulkan/VulkanSwapChain.h"

namespace gfx
{
    auto SwapChain::Create(Window* m_window) -> OwnedPtr<SwapChain>
    {
        auto backendType = gfx::GetBackendType();
        switch (backendType)
        {
            case BackendType::eVulkan: return CreateOwned<VulkanSwapChain>(m_window);
            case BackendType::eNone:
            default: break;
        }
        return nullptr;
    }
}
