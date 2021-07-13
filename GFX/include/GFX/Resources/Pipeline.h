#pragma once

#include "GFX/Core/Base.h"
#include "VertexLayout.h"

namespace gfx
{
    class Framebuffer;
    class Shader;

    enum class PrimitiveTopology
    {
        eNone = 0,
        ePoints,
        eLines,
        eTriangles
    };

    enum class FaceCullMode
    {
        eNone = 0,
        eBack,
        eFront,
        eBoth
    };

    struct PipelineDesc
    {
        Shader* Shader;
        VertexLayout Layout;
        Framebuffer* Framebuffer;
        PrimitiveTopology Topology = PrimitiveTopology::eTriangles;
        FaceCullMode CullMode = FaceCullMode::eBack;
        bool DepthTest = true;
        bool DepthWrite = true;
        bool Wireframe = false;
        float LineWidth = 1.0f;

        bool DepthBias = false;
        float DepthBiasConstantFactor = 4.0f;
        bool DepthBiasSlopeFactor = 1.5f;
    };

    class Pipeline
    {
    public:
        static auto Create(const PipelineDesc& desc) -> OwnedPtr<Pipeline>;

        virtual ~Pipeline() = default;
    };
}
