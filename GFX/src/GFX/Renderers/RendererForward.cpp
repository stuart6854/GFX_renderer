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
        m_deviceContext.ProcessWindowChanges(surface, surface.GetSurfaceWidth(), surface.GetSurfaceHeight());

        m_uniformBufferSet = std::make_shared<UniformBufferSet>(Config::FramesInFlight);
        m_uniformBufferSet->Create(sizeof(UBCamera), 0);
        m_uniformBufferSet->Create(sizeof(UBScene), 2);
        m_uniformBufferSet->Create(sizeof(UBPointLights), 4);

        {
            m_geometryShader = m_deviceContext.CreateShader("resources/shaders/PBR_Static.glsl");

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

    void RendererForward::BeginScene(const Camera& camera, const LightEnvironment& lightEnvironment)
    {
        auto currentFrameIndex = m_deviceContext.GetCurrentFrameIndex();

        /*
         * Update uniform Buffers
         * */
        auto& cameraData = CameraData;
        auto& pointLightsData = PointLightsData;
        auto& sceneData = SceneData;

        auto cameraPosition = glm::inverse(camera.ViewMatrix)[3];

        auto viewProjection = camera.ProjectionMatrix * camera.ViewMatrix;
        cameraData.ViewProjection = viewProjection;
        m_uniformBufferSet->Get(0, 0, currentFrameIndex)->SetData(&cameraData, sizeof(cameraData));

        auto& pointLights = lightEnvironment.PointLights;
        pointLightsData.Count = pointLights.size();
        std::memcpy(pointLightsData.PointLights, pointLights.data(), sizeof(PointLight) * pointLightsData.Count);
        m_uniformBufferSet->Get(4, 0, currentFrameIndex)->SetData(&pointLightsData, 16ull + sizeof(PointLight) * pointLightsData.Count);

        const auto& directionalLight = lightEnvironment.DirectionalLights[0];
        sceneData.Light.Direction = directionalLight.Direction;
        sceneData.Light.Radiance = directionalLight.Radiance;
        sceneData.Light.Multiplier = directionalLight.Multiplier;
        sceneData.CameraPosition = cameraPosition;
        m_uniformBufferSet->Get(2, 0, currentFrameIndex)->SetData(&sceneData, sizeof(sceneData));

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

            auto& materials = drawCall.mesh->GetMaterials();
            for (auto& material : materials)
            {
                UpdateMaterialForRendering(material, m_uniformBufferSet);
            }

            auto& submeshes = drawCall.mesh->GetSubmeshes();
            for (const auto& submesh : submeshes)
            {
                auto& material = drawCall.mesh->GetMaterials()[submesh.MaterialIndex];
                auto layout = m_geometryPipeline->GetAPIPipelineLayout();
                auto descriptorSet = material->GetDescriptorSet(m_deviceContext.GetCurrentFrameIndex());

                std::vector<vk::DescriptorSet> descriptorSets = {
                    descriptorSet,
                    // m_rendererDescriptorSet
                };

                m_renderContext.BindDescriptorSets(vk::PipelineBindPoint::eGraphics, layout, 0, descriptorSets, {});

                const auto modelTransform = drawCall.transform * submesh.Transform;

                auto& buffer = material->GetUniformStorageBuffer();
                m_renderContext.PushConstants(ShaderStage::eVertex, 0, sizeof(glm::mat4), &modelTransform);
                m_renderContext.PushConstants(ShaderStage::ePixel, sizeof(glm::mat4), buffer.Size, buffer.Data);

                m_renderContext.DrawIndexed(submesh.IndexCount, 1, submesh.BaseIndex, submesh.BaseVertex, 0);
            }
        }

        m_renderContext.EndRenderPass();

        m_geometryDrawCalls.clear();
    }

    auto RendererForward::CreateOrRetrieveUniformBufferWriteDescriptors(const std::shared_ptr<Material>& material,
                                                                        const std::shared_ptr<UniformBufferSet>& uniformBufferSet)
        -> const std::vector<std::vector<vk::WriteDescriptorSet>>&
    {
        // Check for existing writeDescriptors
        auto shaderHash = material->GetShader()->GetHash();
        if (m_uniformBufferWriteDescriptorCache.find(uniformBufferSet.get()) != m_uniformBufferWriteDescriptorCache.end())
        {
            const auto& shaderMap = m_uniformBufferWriteDescriptorCache.at(uniformBufferSet.get());
            if (shaderMap.find(shaderHash) != shaderMap.end())
            {
                const auto& writeDescriptors = shaderMap.at(shaderHash);
                return writeDescriptors;
            }
        }

        // Create & Cache writeDescriptors
        auto shader = material->GetShader();
        if (shader->HasDescriptorSet(0))
        {
            const auto& shaderDescriptorsSets = shader->GetShaderDescriptorSets();
            if (!shaderDescriptorsSets.empty())
            {
                auto& writeDescriptors = m_uniformBufferWriteDescriptorCache[uniformBufferSet.get()][shaderHash];
                writeDescriptors.resize(Config::FramesInFlight);
                for (auto&& [binding, shaderUB] : shaderDescriptorsSets[0].UniformBuffers)
                {
                    for (uint32_t frame = 0; frame < Config::FramesInFlight; frame++)
                    {
                        auto uniformBuffer = uniformBufferSet->Get(binding, 0, frame);

                        vk::WriteDescriptorSet writeDesc{};
                        writeDesc.setDescriptorCount(1);
                        writeDesc.setDescriptorType(vk::DescriptorType::eUniformBuffer);
                        writeDesc.setBufferInfo(uniformBuffer->GetDescriptorBufferInfo());
                        writeDesc.setDstBinding(uniformBuffer->GetBinding());
                        writeDescriptors[frame].push_back(writeDesc);
                    }
                }
            }
        }

        return m_uniformBufferWriteDescriptorCache[uniformBufferSet.get()][shaderHash];
    }

    void RendererForward::UpdateMaterialForRendering(const std::shared_ptr<Material>& material, const std::shared_ptr<UniformBufferSet>& uniformBufferSet)
    {
        if (uniformBufferSet != nullptr)
        {
            auto writeDescriptors = CreateOrRetrieveUniformBufferWriteDescriptors(material, uniformBufferSet);

            material->UpdateForRendering(m_deviceContext.GetCurrentFrameIndex(), writeDescriptors);
        }
        else
        {
            material->UpdateForRendering(m_deviceContext.GetCurrentFrameIndex());
        }
    }

}  // namespace gfx