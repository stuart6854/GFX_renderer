//
// Created by stumi on 07/06/21.
//

#include <ExampleBase/ExampleBase.h>

#include <GFX/GFX.h>

#include <iostream>

/*class HelloWindow : public example::ExampleBase
{
public:
    HelloWindow() : ExampleBase("Hello Window") {}

protected:
    void Init_() override
    {
        m_renderSurface = std::make_shared<gfx::RenderSurface>(*m_window);

        m_deviceContext = std::make_shared<gfx::DeviceContext>();

        m_framebuffer = std::make_shared<gfx::Framebuffer>(m_renderSurface.get());

        m_renderContext = std::make_shared<gfx::RenderContext>();
    }

    void Update_(float delta) override {}

    void Render_() override
    {
        m_renderSurface->NewFrame();

        m_renderContext->Begin();

        m_renderContext->BeginRenderPass(gfx::Color(1.0f, 0.0f, 0.0f), m_framebuffer.get());

        m_renderContext->EndRenderPass();
        m_renderContext->End();

        m_renderSurface->Submit(*m_renderContext);
        m_renderSurface->Present();
    }

private:
    std::shared_ptr<gfx::RenderSurface> m_renderSurface;

    std::shared_ptr<gfx::DeviceContext> m_deviceContext;
    std::shared_ptr<gfx::RenderContext> m_renderContext;

    std::shared_ptr<gfx::Framebuffer> m_framebuffer;
};*/

int main(int argc, char** argv)
{
    std::cout << "Running example \"HelloWindow\"" << std::endl;

    /*gfx::Init();
    {
        HelloWindow example;
        example.Run();
    }
    gfx::Shutdown();*/

    gfx::SetDebugCallback([](gfx::DebugLevel level, std::string msg)
    {
        if (level <= gfx::DebugLevel::eWarn)
            std::cout << "[GFX] " << msg << std::endl;
        else
            std::cerr << "[GFX] " << msg << std::endl;
    });

    gfx::Init(gfx::BackendType::eVulkan);

    {
        gfx::Window window(720, 480, "Hello Window");

        auto framebuffer = gfx::Framebuffer::Create(window.GetSwapChain());
        auto cmdBuffer = gfx::CommandBuffer::Create();

        while (!window.IsCloseRequested())
        {
            window.PollEvents();
            window.GetSwapChain()->NewFrame();

            cmdBuffer->Begin();
            cmdBuffer->BeginRenderPass(framebuffer.get());
            cmdBuffer->EndRenderPass();
            cmdBuffer->End();

            window.GetSwapChain()->Present(cmdBuffer.get());
        }
    }
    gfx::Shutdown();

    return 0;
}
