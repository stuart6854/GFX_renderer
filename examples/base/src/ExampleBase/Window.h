//
// Created by stumi on 07/06/21.
//

#ifndef PERSONAL_RENDERER_WINDOW_H
#define PERSONAL_RENDERER_WINDOW_H

#include <cstdint>
#include <string>

#include <GFX/Surface.h>

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

namespace example
{
    class Window : public gfx::ISurface
    {
    public:
        Window(std::string title = "ExampleBase", int w = 720, int h = 480);
        ~Window();

        auto GetWidth() const -> uint32_t { return m_width; }
        auto GetHeight() const -> uint32_t { return m_height; }

        auto ShouldClose() const -> bool;

        void PollEvents();

        auto CreateSurface(vk::Instance instance) -> vk::SurfaceKHR override;

    private:
        std::string m_title;
        uint32_t m_width;
        uint32_t m_height;

        GLFWwindow* m_handle;
    };
}  // namespace example

#endif  // PERSONAL_RENDERER_WINDOW_H
