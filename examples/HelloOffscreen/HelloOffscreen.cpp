//
// Created by stumi on 07/06/21.
//

#define GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <ExampleBase/ExampleBase.h>

#include <GFX/GFX.h>

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <chrono>

struct Vertex
{
    glm::vec3 Position;
    glm::vec2 TexCoord;
};

const std::string vertexSource = R"(
#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

struct VertexOutput
{
    vec2 TexCoord;
};

layout (location = 0) out VertexOutput Output;

void main()
{
    Output.TexCoord = a_TexCoord;

    gl_Position = vec4(a_Position, 1.0f);
}
)";
const std::string pixelSource = R"(
#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform sampler2D u_Texture;

layout (location = 0) out vec4 out_Color;

struct VertexOutput
{
    vec2 TexCoord;
};

layout(location = 0) in VertexOutput Input;

void main()
{
    out_Color = texture(u_Texture, Input.TexCoord);
}
)";

struct Camera
{
    glm::mat4 ViewProj = glm::mat4(1.0f);
};

int main(int argc, char** argv)
{
    GFX_INFO("Running example \"HelloOffscreen\"");

    gfx::SetDebugCallback([](gfx::DebugLevel level, std::string msg)
    {
        if (level <= gfx::DebugLevel::eWarn)
            std::cout << "[GFX] " << msg << std::endl;
        else
            std::cerr << "[GFX] " << msg << std::endl;
    });

    gfx::Init(gfx::BackendType::eVulkan);

    {
        gfx::Window window(1080, 720, "Hello Offscreen");
        auto framebuffer = gfx::Framebuffer::Create(window.GetSwapChain());

        float aspect = float(window.GetWidth()) / float(window.GetHeight());

        const std::vector<Vertex> triVerts = {
            { { -0.5f, -0.5f * aspect, 0.0f }, { 0, 0 } },
            { { -0.5f, 0.5f * aspect, 0.0f }, { 0, 1 } },
            { { 0.5f, 0.5f * aspect, 0.0f }, { 1, 1 } },
            { { 0.5f, -0.5f * aspect, 0.0f }, { 1, 0 } },
        };
        const std::vector<uint32_t> triIndices = { 0, 1, 2, 2, 3, 0 };

        auto vertexBuffer = gfx::Buffer::CreateVertex(sizeof(Vertex) * triVerts.size(), triVerts.data());
        auto indexBuffer = gfx::Buffer::CreateIndex(sizeof(uint32_t) * triIndices.size(), triIndices.data());

        auto shader = gfx::Shader::Create(vertexSource, pixelSource);

        gfx::PipelineDesc pipelineDesc{};
        pipelineDesc.Framebuffer = framebuffer.get();
        pipelineDesc.Shader = shader.get();
        pipelineDesc.Layout = {
            { gfx::ShaderDataType::Float3, "a_Position" },
            { gfx::ShaderDataType::Float2, "a_TexCoord" },
        };
        // pipelineDesc.CullMode = gfx::FaceCullMode::eBack;
        auto pipeline = gfx::Pipeline::Create(pipelineDesc);

        gfx::TextureImporter textureImporter("resources/texture.jpg");
        auto texture = gfx::Texture::Create(textureImporter);

        auto resourceSet = shader->CreateResourceSet(0);
        resourceSet->SetTextureSampler(0, texture.get());
        resourceSet->UpdateBindings();

        gfx::Viewport viewport{};
        viewport.Width = window.GetWidth();
        viewport.Height = window.GetHeight();
        gfx::Scissor scissor{};
        scissor.Width = window.GetWidth();
        scissor.Height = window.GetHeight();

        auto cmdBuffer = gfx::CommandBuffer::Create();

        using clock = std::chrono::high_resolution_clock;
        auto last = clock::now();
        auto delta = 0.0f;
        float time = 0;
        while (!window.IsCloseRequested())
        {
            {
                auto now = clock::now();
                using ms = std::chrono::duration<float, std::milli>;
                delta = std::chrono::duration_cast<ms>(now - last).count() / 1000.0f;
                last = now;
                time += delta;
            }

            window.PollEvents();
            window.GetSwapChain()->NewFrame();

            cmdBuffer->Begin();
            cmdBuffer->SetViewport(viewport);
            cmdBuffer->SetScissor(scissor);

            cmdBuffer->BeginRenderPass(framebuffer.get());

            cmdBuffer->BindPipeline(pipeline.get());
            cmdBuffer->BindVertexBuffer(vertexBuffer.get());
            cmdBuffer->BindIndexBuffer(indexBuffer.get());

            cmdBuffer->BindResourceSets(0, { resourceSet.get() });

            cmdBuffer->DrawIndexed(triIndices.size(), 1, 0, 0, 0);

            cmdBuffer->EndRenderPass();
            cmdBuffer->End();

            window.GetSwapChain()->Present(cmdBuffer.get());
        }
    }
    gfx::Shutdown();

    return 0;
}
