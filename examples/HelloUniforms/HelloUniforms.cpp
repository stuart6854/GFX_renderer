//
// Created by stumi on 07/06/21.
//

#include <ExampleBase/Window.h>

#include <GFX/GFX.h>
#include <GFX/Debug.h>
#include <GFX/DeviceContext.h>
#include <GFX/RenderContext.h>
#include <GFX/Resources/Vertex.h>
#include <GFX/Resources/Buffer.h>
#include <GFX/Resources/Shader.h>
#include <GFX/Resources/Pipeline.h>

#include <iostream>
#include <vector>

const std::vector<gfx::Vertex> triVerts = {
    { { 0.0f, -1.0f, 0.0f }, { 1, 0, 0 } },
    { { -1.0f, 1.0f, 0.0f }, { 0, 0, 1 } },
    { { 1.0f, 1.0f, 0.0f }, { 0, 1, 0 } },
};
const std::vector<uint32_t> triIndices = { 0, 1, 2 };

int main(int argc, char** argv)
{
    GFX_INFO("Running example \"HelloUniforms\"");

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
        auto indexBuffer = deviceContext.CreateBuffer(indexBufferDesc);

        deviceContext.Upload(vertexBuffer.get(), &triVerts[0]);
        deviceContext.Upload(indexBuffer.get(), &triIndices[0]);

        auto shader = std::make_shared<gfx::Shader>("resources/shaders/uniform_shader.glsl");

        gfx::PipelineDesc pipelineDesc;
        pipelineDesc.Shader = shader;
        pipelineDesc.Layout = {
            { gfx::ShaderDataType::Float3, "a_Position" },
            { gfx::ShaderDataType::Float3, "a_Color" },
        };
        pipelineDesc.Framebuffer = deviceContext.GetFramebuffer();

        auto pipeline = deviceContext.CreatePipeline(pipelineDesc);

        gfx::RenderContext renderContext;

        while (!window.ShouldClose())
        {
            window.PollEvents();

            deviceContext.NewFrame();

            renderContext.Begin();

            auto framebuffer = deviceContext.GetFramebuffer();
            renderContext.BeginRenderPass(gfx::Color(0.156f, 0.176f, 0.196f), framebuffer.get());

            renderContext.BindPipeline(pipeline.get());

            renderContext.BindVertexBuffer(vertexBuffer.get());
            renderContext.BindIndexBuffer(indexBuffer.get());

            float pos = 0.5f;
            renderContext.PushConstants(gfx::ShaderStage::eVertex, 0, sizeof(float), &pos);

            renderContext.DrawIndexed(3);

            renderContext.EndRenderPass();
            renderContext.End();

            deviceContext.Submit(renderContext);
            deviceContext.Present();
        }
    }
    gfx::Shutdown();

    return 0;
}