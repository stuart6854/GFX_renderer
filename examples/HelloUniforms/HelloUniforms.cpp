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

const std::vector<Vertex> triVerts = {
    { { -0.5f, 0.5f, 0.0f }, { 0, 0 } },
    { { -0.5f, -0.5f, 0.0f }, { 0, 1 } },
    { { 0.5f, -0.5f, 0.0f }, { 1, 1 } },
    { { 0.5f, 0.5f, 0.0f }, { 1, 0 } },
};
const std::vector<uint32_t> triIndices = { 0, 1, 2, 2, 3, 0 };

const std::string vertexSource = R"(
#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

layout(push_constant) uniform PushBlock
{
    float time;
} pushBlock;

layout(std140, binding = 0) uniform Camera
{
    mat4 ViewProj;
};

struct VertexOutput
{
    vec2 TexCoord;
};

layout (location = 0) out VertexOutput Output;

void main()
{
    Output.TexCoord = a_TexCoord;

    vec3 offset = vec3(cos(pushBlock.time * 0.5) * 0.5, cos(pushBlock.time * 3) * 0.5, 0);
    vec3 pos = a_Position + offset;
    gl_Position = ViewProj * vec4(pos, 1.0f);
}
)";
const std::string pixelSource = R"(
#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D u_Texture;

layout (location = 0) out vec4 out_Color;

struct VertexOutput
{
    vec2 TexCoord;
};

layout(location = 0) in VertexOutput Input;

void main()
{
    out_Color = texture(u_Texture, Input.TexCoord);
    //out_Color = vec4(1, 0, 0, 1);
}
)";

struct Camera
{
    glm::mat4 ViewProj = glm::mat4(1.0f);
};

int main(int argc, char** argv)
{
    GFX_INFO("Running example \"HelloUniforms\"");

    /*gfx::Init();
    {
        example::Window window("HelloUniforms", 1080, 720);

        gfx::RenderSurface renderSurface(window);

        gfx::DeviceContext deviceContext;

        auto framebuffer = std::make_shared<gfx::Framebuffer>(&renderSurface);

        const uint32_t vertexBufferSize = sizeof(gfx::Vertex) * triVerts.size();
        gfx::BufferDesc vertexBufferDesc = { .Type = gfx::BufferType::eVertex, .Size = vertexBufferSize };
        auto vertexBuffer = deviceContext.CreateBuffer(vertexBufferDesc);

        const uint32_t indexBufferSize = sizeof(uint32_t) * triIndices.size();
        gfx::BufferDesc indexBufferDesc = { .Type = gfx::BufferType::eIndex, .Size = indexBufferSize };
        auto indexBuffer = deviceContext.CreateBuffer(indexBufferDesc);

        deviceContext.Upload(vertexBuffer.get(), &triVerts[0]);
        deviceContext.Upload(indexBuffer.get(), &triIndices[0]);

        auto shader = std::make_shared<gfx::Shader>("resources/shaders/hello_uniform.glsl", true);

        gfx::PipelineDesc pipelineDesc;
        pipelineDesc.Shader = shader;
        pipelineDesc.Layout = {
            { gfx::ShaderDataType::Float3, "a_Position" }, { gfx::ShaderDataType::Float3, "a_Normal" },    { gfx::ShaderDataType::Float2, "a_TexCoord" },
            { gfx::ShaderDataType::Float3, "a_Tangent" },  { gfx::ShaderDataType::Float3, "a_Bitangent" },
        };
        pipelineDesc.Framebuffer = framebuffer;

        auto pipeline = deviceContext.CreatePipeline(pipelineDesc);

        gfx::RenderContext renderContext;

        while (!window.ShouldClose())
        {
            window.PollEvents();

            renderSurface.NewFrame();

            renderContext.Begin();
            renderContext.BeginRenderPass(gfx::Color(0.156f, 0.176f, 0.196f), framebuffer.get());

            renderContext.BindPipeline(pipeline.get());

            renderContext.BindVertexBuffer(vertexBuffer.get());
            renderContext.BindIndexBuffer(indexBuffer.get());

            float pos = 0.5f;
            renderContext.PushConstants(gfx::ShaderStage::eVertex, 0, sizeof(float), &pos);

            renderContext.DrawIndexed(3, 1, 0, 0, 0);

            renderContext.EndRenderPass();
            renderContext.End();

            renderSurface.Submit(renderContext);
            renderSurface.Present();
        }
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
        gfx::Window window(1080, 720, "Hello Triangle");
        auto framebuffer = gfx::Framebuffer::Create(window.GetSwapChain());

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
        pipelineDesc.CullMode = gfx::FaceCullMode::eBack;
        auto pipeline = gfx::Pipeline::Create(pipelineDesc);

        glm::mat4 view = glm::lookAt(glm::vec3(-2, 0, -3), { 0, 0, 0 }, { 0, 1, 0 });
        glm::mat4 proj = glm::perspectiveFov(glm::radians(60.0f), float(window.GetWidth()), float(window.GetHeight()), 0.01f, 100.0f);
        proj[1][1] *= -1.0f;

        Camera camera{};
        camera.ViewProj = proj * view;

        auto uniformBufferSet = gfx::UniformBufferSet::Create(gfx::Config::FramesInFlight);
        uniformBufferSet->Create(sizeof(Camera), 0);

        gfx::TextureImporter textureImporter("resources/texture.jpg");
        auto texture = gfx::Texture::Create(textureImporter);

        // auto resourceSet = gfx::ResourceSet::Create(resourceSetLayout.get());
        auto resourceSet = shader->CreateResourceSet(0);
        resourceSet->SetUniformBuffer(0, uniformBufferSet->Get(0));
        resourceSet->SetTextureSampler(1, texture.get());
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

            uniformBufferSet->Get(0, 0, window.GetSwapChain()->GetFrameIndex())->SetData(0, sizeof(Camera), &camera);

            cmdBuffer->Begin();
            cmdBuffer->SetViewport(viewport);
            cmdBuffer->SetScissor(scissor);

            cmdBuffer->BeginRenderPass(framebuffer.get());

            cmdBuffer->BindPipeline(pipeline.get());
            cmdBuffer->BindVertexBuffer(vertexBuffer.get());
            cmdBuffer->BindIndexBuffer(indexBuffer.get());

            cmdBuffer->SetConstants(gfx::ShaderStage::eVertex, 0, sizeof(float), &time);
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
