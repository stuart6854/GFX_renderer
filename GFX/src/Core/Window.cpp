#include "GFX/Core/Window.h"

namespace gfx
{
    Window::Window(uint32_t width, uint32_t height, const std::string& title)
        : m_width(width),
          m_height(height),
          m_title(title)
    {
        if (!glfwInit())
        {
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_handle = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);

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
