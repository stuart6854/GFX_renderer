#pragma once

#include "Base.h"

namespace gfx
{
    class Window;
    class CommandBuffer;

    class SwapChain
    {
    public:
        static auto Create(Window* m_window) -> OwnedPtr<SwapChain>;

        virtual ~SwapChain() = default;

        virtual auto GetWidth() const -> uint32_t = 0;
        virtual auto GetHeight() const -> uint32_t = 0;

        virtual void Recreate(uint32_t width, uint32_t height) = 0;

        virtual void NewFrame() = 0;
        virtual void Present(CommandBuffer* cmdBuffer) = 0;
    };
}
