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

        glfwSetWindowUserPointer(m_handle, this);

        SetupCallbacks();

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
        // m_swapChain->Present();
    }

    void Window::SetupCallbacks() const
    {
        glfwSetKeyCallback(m_handle,
                           [](GLFWwindow* handle,
                              const int key,
                              const int scanCode,
                              const int action,
                              const int mods)
                           {
                                if(key == GLFW_KEY_UNKNOWN) return;

                               auto* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
                               window->m_keyCallback(key, scanCode, action, mods);
                           });

        glfwSetMouseButtonCallback(m_handle,
                                   [](GLFWwindow* handle, const int button, const int action, const int mods)
                                   {
                                       auto* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
                                       window->m_mouseBtnCallback(button, action, mods);
                                   });

        glfwSetCursorPosCallback(m_handle,
                                 [](GLFWwindow* handle, const double xPos, const double yPos)
                                 {
                                     auto* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
                                     window->m_cursorPosCallback(xPos, yPos);
                                 });
    }
}
