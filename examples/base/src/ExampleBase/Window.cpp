//
// Created by stumi on 07/06/21.
//

#include "Window.h"

#include <iostream>
#include <utility>

namespace example
{
    Window::Window(std::string title, int w, int h) : m_title(std::move(title)), m_width(w), m_height(h)
    {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        m_handle = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
        if (!m_handle)
        {
            std::cerr << "Failed to create GLFW window!" << std::endl;
        }
    }

    Window::~Window()
    {
        glfwDestroyWindow(m_handle);
        glfwTerminate();
    }

    auto Window::ShouldClose() const -> bool { return glfwWindowShouldClose(m_handle); }

    void Window::PollEvents() { glfwPollEvents(); }

    auto Window::CreateSurface(vk::Instance instance) -> vk::SurfaceKHR
    {
        VkSurfaceKHR rawSurface{};
        auto result = glfwCreateWindowSurface(instance, m_handle, nullptr, &rawSurface);
        return vk::SurfaceKHR(rawSurface);
    }

}  // namespace example