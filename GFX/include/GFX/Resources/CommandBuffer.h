#pragma once

#include "GFX/Core/Base.h"

#include <cstdint>

namespace gfx
{
    class SwapChain;
    class Framebuffer;

    class CommandBuffer
    {
    public:
        static auto Create(uint32_t count = 0) -> OwnedPtr<CommandBuffer>;

        virtual void Begin() = 0;
        virtual void End() = 0;

        virtual void BeginRenderPass(Framebuffer* framebuffer) = 0;
        virtual void EndRenderPass() = 0;

    };
}