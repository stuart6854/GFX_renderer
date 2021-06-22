//
// Created by stumi on 07/06/21.
//

#ifndef PERSONAL_RENDERER_RENDERERFORWARD_H
#define PERSONAL_RENDERER_RENDERERFORWARD_H

#include "RendererStructures.h"
#include "GFX/DeviceContext.h"
#include "GFX/RenderContext.h"
#include "GFX/Resources/UniformBufferSet.h"
#include "GFX/Resources/Material.h"

#include <glm/mat4x4.hpp>

#include <memory>
#include <vector>

namespace gfx
{
    class ISurface;
    class Mesh;
    class Pipeline;

    class RendererForward
    {
    public:
        void Init(ISurface& surface);

        auto LoadMesh(const std::string& path) -> std::shared_ptr<Mesh>;

        void BeginScene(const Camera& camera);
        void EndScene();

        void DrawMesh(const DrawCall& drawCall);

    private:
        void Flush();

        auto CreateOrRetrieveUniformBufferWriteDescriptors(const std::shared_ptr<Material>& material, const std::shared_ptr<UniformBufferSet>& uniformBufferSet)
            -> const std::vector<std::vector<vk::WriteDescriptorSet>>&;
        void UpdateMaterialForRendering(const std::shared_ptr<Material>& material, const std::shared_ptr<UniformBufferSet>& uniformBufferSet);

    private:
        DeviceContext m_deviceContext;
        RenderContext m_renderContext;

        std::shared_ptr<Shader> m_geometryShader;
        std::shared_ptr<Pipeline> m_geometryPipeline;

        std::vector<DrawCall> m_geometryDrawCalls;
        std::vector<DrawCall> m_shadowDrawCalls;

        std::shared_ptr<UniformBufferSet> m_uniformBufferSet;

        struct UBCamera
        {
            glm::mat4 ViewProjection;
        } CameraData;

        // UniformBufferSet* -> Shader Hash -> Frame -> WriteDescriptor
        std::unordered_map<UniformBufferSet*, std::unordered_map<uint64_t, std::vector<std::vector<vk::WriteDescriptorSet>>>>
            m_uniformBufferWriteDescriptorCache;
    };
}  // namespace gfx

#endif  // PERSONAL_RENDERER_RENDERERFORWARD_H
