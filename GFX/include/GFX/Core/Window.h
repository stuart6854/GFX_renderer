#pragma once

#include "SwapChain.h"

#include <GLFW/glfw3.h>

#include <cstdint>
#include <string>

namespace gfx
{
    class SwapChain;

    class Window
    {
    public:
        Window(uint32_t width, uint32_t height, const std::string& title);
        ~Window();

        auto GetWidth() const -> uint32_t { return m_width; }
        auto Getheight() const -> uint32_t { return m_height; }

        auto GetTitle() const -> const std::string&
        {
            return m_title;
        }

        auto GetHandle() const -> GLFWwindow* { return m_handle; }

        auto GetSwapChain() -> SwapChain* { return m_swapChain.get(); }

        bool IsCloseRequested() const;

        void PollEvents();
        void Present();

    private:
        uint32_t m_width;
        uint32_t m_height;
        std::string m_title;

        GLFWwindow* m_handle;

        OwnedPtr<SwapChain> m_swapChain;
    };
}
