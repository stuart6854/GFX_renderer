//
// Created by stumi on 07/06/21.
//

#ifndef PERSONAL_RENDERER_RENDERERFORWARD_H
#define PERSONAL_RENDERER_RENDERERFORWARD_H

#include "DrawCall.h"
#include "GFX/DeviceContext.h"
#include "GFX/RenderContext.h"

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

        void BeginScene();
        void EndScene();

        void DrawMesh(const DrawCall& drawCall);

    private:
        void Flush();

    private:
        DeviceContext m_deviceContext;
        RenderContext m_renderContext;

        std::shared_ptr<Shader> m_geometryShader;
        std::shared_ptr<Pipeline> m_geometryPipeline;

        std::vector<DrawCall> m_geometryDrawCalls;
        std::vector<DrawCall> m_shadowDrawCalls;
    };
}  // namespace gfx

#endif  // PERSONAL_RENDERER_RENDERERFORWARD_H
