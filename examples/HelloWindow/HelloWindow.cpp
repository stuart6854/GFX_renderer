//
// Created by stumi on 07/06/21.
//

#include <ExampleBase/Window.h>

#include <GFX/GFX.h>
#include <GFX/Core/RenderSurface.h>
#include <GFX/DeviceContext.h>
#include <GFX/RenderContext.h>

#include <iostream>

int main(int argc, char** argv)
{
    std::cout << "Running example \"HelloWindow\"" << std::endl;

    gfx::Init();
    {
        example::Window window;

        gfx::RenderSurface renderSurface(window);

        gfx::DeviceContext deviceContext;

        gfx::Framebuffer framebuffer(&renderSurface);

        gfx::RenderContext renderContext;
        while (!window.ShouldClose())
        {
            window.PollEvents();

            renderSurface.NewFrame();

            renderContext.Begin();

            // auto framebuffer = deviceContext.GetFramebuffer();
            renderContext.BeginRenderPass(gfx::Color(1.0f, 0.0f, 0.0f), &framebuffer);

            renderContext.EndRenderPass();
            renderContext.End();

            renderSurface.Submit(renderContext);
            renderSurface.Present();
        }
    }
    gfx::Shutdown();

    return 0;
}