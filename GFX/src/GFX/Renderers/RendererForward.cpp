//
// Created by stumi on 07/06/21.
//

#include "GFX/Renderers/RendererForward.h"

#include "GFX/Config.h"
#include "GFX/Core/RenderSurface.h"
#include "GFX/DeviceContext.h"
#include "GFX/RenderContext.h"
#include "GFX/Resources/Mesh.h"
#include "GFX/Resources/Shader.h"
#include "GFX/Resources/Material.h"
#include "GFX/Resources/Framebuffer.h"

#include "Platform/Vulkan/VulkanCore.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace gfx
{
    void RendererForward::Init(IWindowSurface& windowSurface)
    {
        m_renderSurface = std::make_shared<RenderSurface>(windowSurface);

        /* Uniform Buffers */

        m_uniformBufferSet = std::make_shared<UniformBufferSet>(Config::FramesInFlight);
        m_uniformBufferSet->Create(sizeof(UBCamera), 0);
        m_uniformBufferSet->Create(sizeof(UBShadow), 1);
        m_uniformBufferSet->Create(sizeof(UBScene), 2);
        m_uniformBufferSet->Create(sizeof(UBPointLights), 4);

        /* Framebuffers */

        {
            // Shadow Pass
            FramebufferDesc framebufferDesc{};
            framebufferDesc.Width = 2048;
            framebufferDesc.Height = 2048;
            framebufferDesc.Attachments = { { ImageFormat::eDepth32F } };

            m_shadowFramebuffer = std::make_shared<Framebuffer>(framebufferDesc);

            ShaderLibrary::Load("resources/shaders/ShadowMap.glsl", true);
            m_shadowShader = ShaderLibrary::Get("ShadowMap");

            PipelineDesc pipelineDesc;
            pipelineDesc.Shader = m_shadowShader;
            pipelineDesc.CullMode = FaceCullMode::eFront;
            pipelineDesc.DepthBias = true;
            pipelineDesc.Layout = {
                { ShaderDataType::Float3, "a_Position" }, { ShaderDataType::Float3, "a_Normal" },    { ShaderDataType::Float2, "a_TexCoord" },
                { ShaderDataType::Float3, "a_Tangent" },  { ShaderDataType::Float3, "a_Bitangent" },
            };
            pipelineDesc.Framebuffer = m_shadowFramebuffer;

            m_shadowPipeline = m_deviceContext->CreatePipeline(pipelineDesc);
            m_shadowMaterial = std::make_shared<Material>(m_shadowShader);
        }

        {
            // Geometry Pass
            m_swapChainFramebuffer = std::make_shared<Framebuffer>(m_renderSurface.get());

            ShaderLibrary::Load("resources/shaders/PBR_Static.glsl", true);
            m_geometryShader = ShaderLibrary::Get("PBR_Static");

            PipelineDesc pipelineDesc;
            pipelineDesc.Shader = m_geometryShader;
            pipelineDesc.Layout = {
                { ShaderDataType::Float3, "a_Position" }, { ShaderDataType::Float3, "a_Normal" },    { ShaderDataType::Float2, "a_TexCoord" },
                { ShaderDataType::Float3, "a_Tangent" },  { ShaderDataType::Float3, "a_Bitangent" },
            };
            pipelineDesc.Framebuffer = m_swapChainFramebuffer;

            m_geometryPipeline = m_deviceContext->CreatePipeline(pipelineDesc);
        }
    }

    auto RendererForward::LoadMesh(const std::string& path) -> std::shared_ptr<Mesh>
    {
        auto mesh = std::make_shared<Mesh>(*m_deviceContext, path);

        // TODO: Move uploads to Mesh.cpp
        m_deviceContext->Upload(mesh->GetVertexBuffer().get(), mesh->GetVertices().data());
        m_deviceContext->Upload(mesh->GetIndexBuffer().get(), mesh->GetIndices().data());

        return mesh;
    }

    void RendererForward::BeginScene(const Camera& camera, const LightEnvironment& lightEnvironment)
    {
        const auto currentFrameIndex = m_renderSurface->GetFrameIndex();

        /*
         * Update uniform Buffers
         * */
        auto& cameraData = CameraData;
        auto& shadowData = ShadowData;
        auto& pointLightsData = PointLightsData;
        auto& sceneData = SceneData;

        const auto cameraPosition = glm::inverse(camera.ViewMatrix)[3];

        const auto viewProjection = camera.ProjectionMatrix * camera.ViewMatrix;
        cameraData.ViewProjection = viewProjection;
        m_uniformBufferSet->Get(0, 0, currentFrameIndex)->SetData(&cameraData, sizeof(cameraData));

        const auto& dirLight = lightEnvironment.DirectionalLights;
        const auto dirLightProj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 20.0f);
        const auto dirLightView = glm::lookAt(dirLight->Position, dirLight->Target, glm::vec3{ 0, 1, 0 });
        shadowData.LightViewProj = dirLightProj * dirLightView;
        m_uniformBufferSet->Get(1, 0, currentFrameIndex)->SetData(&shadowData, sizeof(shadowData));

        auto& pointLights = lightEnvironment.PointLights;
        pointLightsData.Count = pointLights.size();
        std::memcpy(pointLightsData.PointLights, pointLights.data(), sizeof(PointLight) * pointLightsData.Count);
        m_uniformBufferSet->Get(4, 0, currentFrameIndex)->SetData(&pointLightsData, 16ull + sizeof(PointLight) * pointLightsData.Count);

        const auto& directionalLight = lightEnvironment.DirectionalLights[0];
        sceneData.Light.Direction = directionalLight.Position;
        sceneData.Light.Color = directionalLight.Color;
        sceneData.CameraPosition = cameraPosition;
        m_uniformBufferSet->Get(2, 0, currentFrameIndex)->SetData(&sceneData, sizeof(sceneData));

        m_renderSurface->NewFrame();
        m_renderContext->Begin();

        // Wait for command buffer to be finished before updating descriptors
        SetSceneEnvironment(m_shadowFramebuffer->GetDepthImage());
    }

    void RendererForward::EndScene()
    {
        Flush();

        m_renderContext->End();
        m_renderSurface->Submit(*m_renderContext);
        m_renderSurface->Present();
    }

    void RendererForward::DrawMesh(const DrawCall& drawCall)
    {
        // TODO: Culling, sorting, etc.
        m_geometryDrawCalls.emplace_back(drawCall);
        m_shadowDrawCalls.emplace_back(drawCall);
    }

    void RendererForward::Flush()
    {
        ShadowPass();
        GeometryPass();
    }

    void RendererForward::ShadowPass()
    {
        m_renderContext->BeginRenderPass({ 0.0f, 0.0f, 0.0f }, m_shadowFramebuffer.get());

        m_renderContext->BindPipeline(m_shadowPipeline.get());

        for (const auto& drawCall : m_shadowDrawCalls)
        {
            m_renderContext->BindVertexBuffer(drawCall.mesh->GetVertexBuffer().get());
            m_renderContext->BindIndexBuffer(drawCall.mesh->GetIndexBuffer().get());

            UpdateMaterialForRendering(m_shadowMaterial, m_uniformBufferSet);

            const auto layout = m_shadowPipeline->GetAPIPipelineLayout();
            const auto descriptorSet = m_shadowMaterial->GetDescriptorSet(m_renderSurface->GetFrameIndex());
            if (descriptorSet) m_renderContext->BindDescriptorSets(vk::PipelineBindPoint::eGraphics, layout, 0, { descriptorSet }, {});

            auto& buffer = m_shadowMaterial->GetUniformStorageBuffer();
            if (buffer) m_renderContext->PushConstants(ShaderStage::ePixel, sizeof(glm::mat4), buffer.Size, buffer.Data);

            auto& submeshes = drawCall.mesh->GetSubmeshes();
            for (const auto& submesh : submeshes)
            {
                const auto modelTransform = drawCall.transform * submesh.Transform;

                m_renderContext->PushConstants(ShaderStage::eVertex, 0, sizeof(glm::mat4), &modelTransform);

                m_renderContext->DrawIndexed(submesh.IndexCount, 1, submesh.BaseIndex, submesh.BaseVertex, 0);
            }
        }

        m_renderContext->EndRenderPass();

        m_shadowDrawCalls.clear();
    }

    void RendererForward::GeometryPass()
    {
        m_renderContext->BeginRenderPass({ 0.156f, 0.176f, 0.196f }, m_swapChainFramebuffer.get());

        m_renderContext->BindPipeline(m_geometryPipeline.get());

        for (const auto& drawCall : m_geometryDrawCalls)
        {
            m_renderContext->BindVertexBuffer(drawCall.mesh->GetVertexBuffer().get());
            m_renderContext->BindIndexBuffer(drawCall.mesh->GetIndexBuffer().get());

            auto& materials = drawCall.mesh->GetMaterials();
            for (auto& material : materials)
            {
                UpdateMaterialForRendering(material, m_uniformBufferSet);
            }

            auto& submeshes = drawCall.mesh->GetSubmeshes();
            for (const auto& submesh : submeshes)
            {
                auto& material = drawCall.mesh->GetMaterials()[submesh.MaterialIndex];
                const auto layout = m_geometryPipeline->GetAPIPipelineLayout();
                const auto descriptorSet = material->GetDescriptorSet(m_renderSurface->GetFrameIndex());

                std::vector<vk::DescriptorSet> descriptorSets = { descriptorSet, m_activeRendererDescriptorSet };

                m_renderContext->BindDescriptorSets(vk::PipelineBindPoint::eGraphics, layout, 0, descriptorSets, {});

                const auto modelTransform = drawCall.transform * submesh.Transform;

                auto& buffer = material->GetUniformStorageBuffer();
                m_renderContext->PushConstants(ShaderStage::eVertex, 0, sizeof(glm::mat4), &modelTransform);
                m_renderContext->PushConstants(ShaderStage::ePixel, sizeof(glm::mat4), buffer.Size, buffer.Data);

                m_renderContext->DrawIndexed(submesh.IndexCount, 1, submesh.BaseIndex, submesh.BaseVertex, 0);
            }
        }

        m_renderContext->EndRenderPass();

        m_geometryDrawCalls.clear();
    }

    void RendererForward::SetSceneEnvironment(const std::shared_ptr<Image>& shadowMap)
    {
        // TODO: Make API agnostic

        const auto frameIndex = m_renderSurface->GetFrameIndex();

        if (m_rendererDescriptorSet.empty())
        {
            m_rendererDescriptorSet.resize(Config::FramesInFlight);
            for (uint32_t i = 0; i < Config::FramesInFlight; i++) m_rendererDescriptorSet.at(i) = m_geometryShader->CreateDescriptorSet(1, frameIndex);
        }

        const auto descriptorSet = m_rendererDescriptorSet.at(frameIndex).DescriptorSets[0];
        m_activeRendererDescriptorSet = descriptorSet;

        std::array<vk::WriteDescriptorSet, 1> writeDescriptors;

        writeDescriptors[0] = *m_geometryShader->GetDescriptorSet("u_ShadowMapTexture", 1);
        writeDescriptors[0].dstSet = descriptorSet;
        const auto& shadowImageInfo = shadowMap->GetVulkanDescriptorInfo();
        writeDescriptors[0].pImageInfo = &shadowImageInfo;

        const auto device = Vulkan::GetDevice();
        device.updateDescriptorSets(writeDescriptors, {});
    }

    auto RendererForward::CreateOrRetrieveUniformBufferWriteDescriptors(const std::shared_ptr<Material>& material,
                                                                        const std::shared_ptr<UniformBufferSet>& uniformBufferSet)
        -> const std::vector<std::vector<vk::WriteDescriptorSet>>&
    {
        // Check for existing writeDescriptors
        const auto shaderHash = material->GetShader()->GetHash();
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
        const auto shader = material->GetShader();
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
                        const auto uniformBuffer = uniformBufferSet->Get(binding, 0, frame);

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
            const auto writeDescriptors = CreateOrRetrieveUniformBufferWriteDescriptors(material, uniformBufferSet);

            material->UpdateForRendering(m_renderSurface->GetFrameIndex(), writeDescriptors);
        }
        else
        {
            material->UpdateForRendering(m_renderSurface->GetFrameIndex());
        }
    }

}  // namespace gfx