#pragma once

#include "SwapChain.h"

#include <GLFW/glfw3.h>

#include <cstdint>
#include <string>
#include <functional>

namespace gfx
{
    class SwapChain;

    class Window
    {
    public:
        Window(uint32_t width, uint32_t height, const std::string& title);
        ~Window();

        auto GetWidth() const -> uint32_t { return m_width; }
        auto GetHeight() const -> uint32_t { return m_height; }

        auto GetTitle() const -> const std::string&
        {
            return m_title;
        }

        auto GetHandle() const -> GLFWwindow* { return m_handle; }

        auto GetSwapChain() -> SwapChain* { return m_swapChain.get(); }

        bool IsCloseRequested() const;

        void PollEvents();
        void Present();

        void SetSizeCallback(const std::function<void(int, int)>& callback) { m_sizeCallback = callback; }
        void SetKeyCallback(const std::function<void(int, int, int, int)>& callback) { m_keyCallback = callback; }
        void SetMouseBtnCallback(const std::function<void(int, int, int)>& callback) { m_mouseBtnCallback = callback; }
        void SetCursorPosCallback(const std::function<void(double, double)>& callback) { m_cursorPosCallback = callback; }

    private:
        void SetupCallbacks() const;

        void Resize(int width, int height);

    private:
        uint32_t m_width;
        uint32_t m_height;
        std::string m_title;

        GLFWwindow* m_handle;

        OwnedPtr<SwapChain> m_swapChain;

        std::function<void(int, int)> m_sizeCallback;
        std::function<void(int, int, int, int)> m_keyCallback;
        std::function<void(int, int, int)> m_mouseBtnCallback;
        std::function<void(double, double)> m_cursorPosCallback;
    };
}
