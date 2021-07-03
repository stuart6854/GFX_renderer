//
// Created by stumi on 07/06/21.
//

#ifndef PERSONAL_RENDERER_RENDERERFORWARD_H
#define PERSONAL_RENDERER_RENDERERFORWARD_H

#include "RendererStructures.h"
#include "GFX/Resources/Shader.h"
#include "GFX/Resources/UniformBufferSet.h"

#include <glm/mat4x4.hpp>

#include <memory>
#include <vector>

namespace gfx
{
    class IWindowSurface;
    class RenderSurface;
    class DeviceContext;
    class RenderContext;
    class Mesh;
    class Pipeline;
    class Material;
    class Image;

    class RendererForward
    {
    public:
        void Init(IWindowSurface& windowSurface);
        void Shutdown();

        auto LoadMesh(const std::string& path) -> std::shared_ptr<Mesh>;

        void BeginScene(const Camera& camera, const LightEnvironment& lightEnvironment);
        void EndScene();

        void DrawMesh(const DrawCall& drawCall);
        void DrawCollider(const DrawCall& drawCall);

        auto GetDeviceContext() const -> std::shared_ptr<DeviceContext> { return m_deviceContext; }

    private:
        void Flush();

        void ShadowPass();
        void GeometryPass();

        void SetSceneEnvironment(const std::shared_ptr<Image>& shadowMap);

        auto CreateOrRetrieveUniformBufferWriteDescriptors(const std::shared_ptr<Material>& material,
                                                           const std::shared_ptr<UniformBufferSet>& uniformBufferSet)
        -> const std::vector<std::vector<vk::WriteDescriptorSet>>&;
        void UpdateMaterialForRendering(const std::shared_ptr<Material>& material, const std::shared_ptr<UniformBufferSet>& uniformBufferSet);

    private:
        std::shared_ptr<RenderSurface> m_renderSurface;
        std::shared_ptr<DeviceContext> m_deviceContext;
        std::shared_ptr<RenderContext> m_renderContext;

        std::vector<DrawCall> m_shadowDrawCalls;
        std::vector<DrawCall> m_geometryDrawCalls;
        std::vector<DrawCall> m_colliderDrawCalls;

        std::shared_ptr<Shader> m_shadowShader;
        std::shared_ptr<Pipeline> m_shadowPipeline;
        std::shared_ptr<Material> m_shadowMaterial;

        std::shared_ptr<Shader> m_geometryShader;
        std::shared_ptr<Pipeline> m_geometryPipeline;
        std::shared_ptr<Pipeline> m_geometryWireframePipeline;

        std::shared_ptr<Material> m_colliderMaterial;

        std::shared_ptr<Framebuffer> m_shadowFramebuffer;
        std::shared_ptr<Framebuffer> m_swapChainFramebuffer;

        std::shared_ptr<UniformBufferSet> m_uniformBufferSet;
        std::vector<Shader::ShaderMaterialDescriptorSet> m_rendererDescriptorSet;
        vk::DescriptorSet m_activeRendererDescriptorSet;

        struct UBCamera
        {
            glm::mat4 ViewProjection;
        } CameraData;

        struct UBShadow
        {
            glm::mat4 LightViewProj;
        } ShadowData;

        struct DirLight
        {
            glm::vec3 Direction;
            float Padding = 0.0f;
            glm::vec3 Color;
            float Padding2;
        };

        struct UBPointLights
        {
            uint32_t Count = 0;
            glm::vec3 Padding = {};
            PointLight PointLights[16] = {};
        } PointLightsData;

        struct UBScene
        {
            DirLight Light;
            glm::vec3 CameraPosition;
        } SceneData;

        // UniformBufferSet* -> Shader Hash -> Frame -> WriteDescriptor
        std::unordered_map<UniformBufferSet*, std::unordered_map<uint64_t, std::vector<std::vector<vk::WriteDescriptorSet>>>>
        m_uniformBufferWriteDescriptorCache;
    };
} // namespace gfx

#endif  // PERSONAL_RENDERER_RENDERERFORWARD_H
