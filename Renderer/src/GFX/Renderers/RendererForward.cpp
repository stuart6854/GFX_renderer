//
// Created by stumi on 07/06/21.
//

#include "GFX/Renderers/RendererForward.h"

#include "GFX/Config.h"
#include "GFX/Resources/Mesh.h"

namespace gfx
{
    void RendererForward::Init(ISurface& surface)
    {
        m_deviceContext.ProcessWindowChanges(surface, 720, 480);

        m_uniformBufferSet = std::make_shared<UniformBufferSet>(Config::FramesInFlight);
        m_uniformBufferSet->Create(sizeof(UBCamera), 0);

        {
            m_geometryShader = m_deviceContext.CreateShader("resources/forwardrenderer/scene_shader.glsl");

            PipelineDesc pipelineDesc;
            pipelineDesc.Shader = m_geometryShader;
            pipelineDesc.Layout = {
                { ShaderDataType::Float3, "a_Position" },
                { ShaderDataType::Float3, "a_Normal" },
                { ShaderDataType::Float2, "a_TexCoord" },
            };
            pipelineDesc.Framebuffer = m_deviceContext.GetFramebuffer();

            m_geometryPipeline = m_deviceContext.CreatePipeline(pipelineDesc);
        }
    }

    auto RendererForward::LoadMesh(const std::string& path) -> std::shared_ptr<Mesh>
    {
        auto mesh = std::make_shared<Mesh>(path);

        m_deviceContext.Upload(mesh->GetVertexBuffer().get(), mesh->GetVertices().data());
        m_deviceContext.Upload(mesh->GetIndexBuffer().get(), mesh->GetIndices().data());

        return mesh;
    }

    void RendererForward::BeginScene(const Camera& camera)
    {
        auto currentFrameIndex = m_deviceContext.GetCurrentFrameIndex();

        /*
         * Update uniform Buffers
         * */
        auto& cameraData = CameraData;

        auto viewProjection = camera.ProjectionMatrix * camera.ViewMatrix;

        cameraData.ViewProjection = viewProjection;

        m_uniformBufferSet->Get(0, 0, currentFrameIndex)->SetData(&cameraData, sizeof(cameraData));

        m_deviceContext.NewFrame();
        m_renderContext.Begin();
    }

    void RendererForward::EndScene()
    {
        Flush();

        m_renderContext.End();
        m_deviceContext.Submit(m_renderContext);
        m_deviceContext.Present();
    }

    void RendererForward::DrawMesh(const DrawCall& drawCall)
    {
        // TODO: Culling, sorting, etc.
        m_geometryDrawCalls.emplace_back(drawCall);
        m_shadowDrawCalls.emplace_back(drawCall);
    }

    void RendererForward::Flush()
    {
        auto framebuffer = m_deviceContext.GetFramebuffer();
        m_renderContext.BeginRenderPass({ 0.156f, 0.176f, 0.196f }, framebuffer.get());

        m_renderContext.BindPipeline(m_geometryPipeline.get());

        for (const auto& drawCall : m_geometryDrawCalls)
        {
            m_renderContext.BindVertexBuffer(drawCall.mesh->GetVertexBuffer().get());
            m_renderContext.BindIndexBuffer(drawCall.mesh->GetIndexBuffer().get());

            m_renderContext.PushConstants(ShaderStage::eVertex, 0, sizeof(glm::mat4), &drawCall.transform);

            m_renderContext.DrawIndexed(drawCall.mesh->GetIndexCount());
        }

        m_renderContext.EndRenderPass();

        m_geometryDrawCalls.clear();
    }

}  // namespace gfx