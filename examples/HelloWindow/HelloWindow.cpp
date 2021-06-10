//
// Created by stumi on 07/06/21.
//

#include <ExampleBase/Window.h>

#include <GFX/GFX.h>
#include <GFX/DeviceContext.h>
#include <GFX/RenderContext.h>

#include <iostream>

int main(int argc, char** argv)
{
    std::cout << "Running example \"HelloWindow\"" << std::endl;

    gfx::Init();
    {
        example::Window window;

        gfx::DeviceContext deviceContext;
        deviceContext.ProcessWindowChanges(window, window.GetWidth(), window.GetHeight());

        gfx::RenderContext renderContext;

        while (!window.ShouldClose())
        {
            window.PollEvents();

            deviceContext.NewFrame();

            renderContext.Begin();
            renderContext.BeginRenderPass(gfx::Color(1.0f, 0.0f, 0.0f), deviceContext.GetFramebuffer());

            renderContext.EndRenderPass();
            renderContext.End();

            deviceContext.Submit(renderContext);
            deviceContext.Present();
        }
    }
    gfx::Shutdown();

    return 0;
}