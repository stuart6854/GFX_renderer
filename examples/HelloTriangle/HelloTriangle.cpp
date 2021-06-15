//
// Created by stumi on 07/06/21.
//

#include <ExampleBase/Window.h>

#include <GFX/GFX.h>
#include <GFX/DeviceContext.h>
#include <GFX/RenderContext.h>
#include <GFX/Resources/Vertex.h>
#include <GFX/Resources/Buffer.h>
#include <GFX/Resources/Shader.h>
#include <GFX/Resources/Pipeline.h>

#include <iostream>
#include <vector>

const std::vector<gfx::Vertex> triVerts = { { { 0.0f, 1.0f, 0.0f } }, { { 1.0f, -1.0f, 0.0f } }, { { -1.0f, -1.0f, 0.0f } } };
const std::vector<uint32_t> triIndices = { 0, 1, 2 };

int main(int argc, char** argv)
{
    std::cout << "Running example \"HelloTriangle\"" << std::endl;

    gfx::Init();
    {
        example::Window window;

        gfx::DeviceContext deviceContext;
        deviceContext.ProcessWindowChanges(window, window.GetWidth(), window.GetHeight());

        const uint32_t vertexBufferSize = sizeof(gfx::Vertex) * triVerts.size();
        gfx::BufferDesc vertexBufferDesc = { .Type = gfx::BufferType::eVertex, .Size = vertexBufferSize };
        auto vertexBuffer = deviceContext.CreateBuffer(vertexBufferDesc);

        const uint32_t indexBufferSize = sizeof(uint32_t) * triIndices.size();
        gfx::BufferDesc indexBufferDesc = { .Type = gfx::BufferType::eIndex, .Size = indexBufferSize };
        gfx::Buffer indexBuffer = deviceContext.CreateBuffer(indexBufferDesc);

        deviceContext.Upload(vertexBuffer, &triVerts[0]);
        deviceContext.Upload(indexBuffer, &triIndices[0]);

        auto shader = std::make_shared<gfx::Shader>("resources/shaders/shader.glsl");

        gfx::PipelineDesc pipelineDesc;
        pipelineDesc.Shader = shader;
        pipelineDesc.Layout = { { gfx::ShaderDataType::Float3, "a_Position" } };
        pipelineDesc.Framebuffer = std::make_shared<gfx::Framebuffer>(deviceContext.GetFramebuffer());

        gfx::Pipeline pipeline(pipelineDesc);

        gfx::RenderContext renderContext;

        while (!window.ShouldClose())
        {
            window.PollEvents();

            deviceContext.NewFrame();

            renderContext.Begin();
            renderContext.BeginRenderPass(gfx::Color(0.156f, 0.176f, 0.196f), deviceContext.GetFramebuffer());

            renderContext.BindVertexBuffer(vertexBuffer);
            renderContext.BindIndexBuffer(indexBuffer);

            renderContext.EndRenderPass();
            renderContext.End();

            deviceContext.Submit(renderContext);
            deviceContext.Present();
        }
    }
    gfx::Shutdown();

    return 0;
}