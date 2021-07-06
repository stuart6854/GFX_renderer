#pragma once

#include "Base.h"

namespace gfx
{
    class Window;

    class SwapChain
    {
    public:
        static auto Create(Window* m_window) -> OwnedPtr<SwapChain>;

        virtual ~SwapChain() = default;

        virtual void Recreate(uint32_t width, uint32_t height) = 0;

        virtual void Present() = 0;
    };
}
