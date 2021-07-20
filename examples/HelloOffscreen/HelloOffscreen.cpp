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

const std::string offscreenVertexSrc = R"(
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
const std::string offscreenPixelSrc = R"(
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
    //out_Color = vec4(1, 0, 0, 1);
}
)";

const std::string blurVertexSrc = R"(
#version 450
#extension GL_ARB_separate_shader_objects : enable

struct VertexOutput
{
    vec2 TexCoord;
};

layout (location = 0) out VertexOutput Output;

void main()
{
    Output.TexCoord = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);

    gl_Position = vec4(Output.TexCoord * 2.0f - 1.0f, 0.0f, 1.0f);
}
)";
const std::string blurPixelSrc = R"(
#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform sampler2D u_Texture;

struct VertexOutput
{
    vec2 TexCoord;
};

layout(location = 0) in VertexOutput Input;

layout (location = 0) out vec4 out_Color;

#define radialOrigin vec2(0.5, 0.5)
#define radialBlurScale 0.35
#define radialBlurStrength 0.75

void main()
{
    ivec2 texDim = textureSize(u_Texture, 0);
    vec2 radialSize = vec2(1.0 / texDim.s, 1.0 / texDim.t);

    vec2 UV = Input.TexCoord;

    vec4 color = vec4(0, 0, 0, 0);
    UV += radialSize * 0.5 - radialOrigin;

    #define samples 32

    for(int i = 0; i < samples; i++)
    {
        float scale = 1.0 - radialBlurScale * (float(i) / float(samples - 1));
        color += texture(u_Texture, UV * scale + radialOrigin);
    }

    out_Color = (color / samples) * radialBlurStrength;
    //out_Color = texture(u_Texture, Input.TexCoord);
    //out_Color = vec4(1, 0, 0, 1);
}
)";

struct OffscreenPass
{
    uint32_t width;
    uint32_t height;
    gfx::OwnedPtr<gfx::Framebuffer> framebuffer;
    gfx::OwnedPtr<gfx::Pipeline> pipeline;
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
        auto swapchainFramebuffer = gfx::Framebuffer::Create(window.GetSwapChain());

        OffscreenPass offscreenPass{};
        offscreenPass.width = 1080;
        offscreenPass.height = 720;

        auto offscreenShader = gfx::Shader::Create(offscreenVertexSrc, offscreenPixelSrc);
        auto blurShader = gfx::Shader::Create(blurVertexSrc, blurPixelSrc);

        {
            /* Offscreen */

            gfx::FramebufferDesc framebufferDesc{};
            framebufferDesc.Width = window.GetWidth();
            framebufferDesc.Height = window.GetHeight();
            framebufferDesc.Attachments = { { gfx::TextureFormat::eRGBA }, { gfx::TextureFormat::eDepth } };

            offscreenPass.framebuffer = gfx::Framebuffer::Create(framebufferDesc);

            gfx::PipelineDesc pipelineDesc{};
            pipelineDesc.Framebuffer = offscreenPass.framebuffer.get();
            pipelineDesc.Shader = offscreenShader.get();
            pipelineDesc.Layout = {
                { gfx::ShaderDataType::Float3, "a_Position" },
                { gfx::ShaderDataType::Float2, "a_TexCoord" },
            };
            offscreenPass.pipeline = gfx::Pipeline::Create(pipelineDesc);
        }

        gfx::OwnedPtr<gfx::Pipeline> blurPipeline;
        {
            /* Swapchain */

            gfx::PipelineDesc pipelineDesc{};
            pipelineDesc.Framebuffer = swapchainFramebuffer.get();
            pipelineDesc.Shader = blurShader.get();
            pipelineDesc.Layout = {};
            pipelineDesc.CullMode = gfx::FaceCullMode::eNone;
            blurPipeline = gfx::Pipeline::Create(pipelineDesc);
        }

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

        gfx::TextureImporter textureImporter("resources/texture.jpg");
        auto texture = gfx::Texture::Create(textureImporter);

        auto offscreenResSet = offscreenShader->CreateResourceSet(0);
        offscreenResSet->SetTextureSampler(0, texture.get());
        offscreenResSet->UpdateBindings();

        auto blurResSet = offscreenShader->CreateResourceSet(0);
        blurResSet->SetTextureSampler(0, offscreenPass.framebuffer->GetColorTexture(0));
        blurResSet->UpdateBindings();

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

            cmdBuffer->BeginRenderPass(offscreenPass.framebuffer.get());
            {
                cmdBuffer->BindPipeline(offscreenPass.pipeline.get());
                cmdBuffer->BindVertexBuffer(vertexBuffer.get());
                cmdBuffer->BindIndexBuffer(indexBuffer.get());

                cmdBuffer->BindResourceSets(0, { offscreenResSet.get() });

                cmdBuffer->DrawIndexed(triIndices.size(), 1, 0, 0, 0);
            }
            cmdBuffer->EndRenderPass();

            cmdBuffer->BeginRenderPass(swapchainFramebuffer.get());
            {
                cmdBuffer->BindPipeline(blurPipeline.get());

                cmdBuffer->BindResourceSets(0, { blurResSet.get() });

                cmdBuffer->Draw(3);
            }
            cmdBuffer->EndRenderPass();
            cmdBuffer->End();

            window.GetSwapChain()->Present(cmdBuffer.get());
        }
    }
    gfx::Shutdown();

    return 0;
}
