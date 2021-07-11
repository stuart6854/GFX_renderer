#include "GFX/Core/Window.h"

#include "GFX/Debug.h"
#include "GFX/Core/SwapChain.h"

namespace gfx
{
    Window::Window(uint32_t width, uint32_t height, const std::string& title)
        : m_width(width),
          m_height(height),
          m_title(title)
    {
        GFX_ASSERT(glfwInit(), "glfwInit() failed!");

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_handle = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
        GFX_ASSERT(m_handle != nullptr, "glfwCreateWindow() failed!");

        m_swapChain = SwapChain::Create(this);
    }

    Window::~Window()
    {
        glfwDestroyWindow(m_handle);
        glfwTerminate();
    }

    bool Window::IsCloseRequested() const
    {
        return glfwWindowShouldClose(m_handle);
    }

    void Window::PollEvents()
    {
        glfwPollEvents();
    }

    void Window::Present()
    {
        m_swapChain->Present();
    }
}
