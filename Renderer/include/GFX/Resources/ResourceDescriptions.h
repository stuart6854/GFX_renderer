//
// Created by stumi on 11/06/21.
//

#ifndef PERSONAL_RENDERER_RESOURCEDESCRIPTIONS_H
#define PERSONAL_RENDERER_RESOURCEDESCRIPTIONS_H

#include "Shader.h"
#include "Framebuffer.h"
#include "VertexLayout.h"

#include <cstdint>

namespace gfx
{
    enum class BufferType
    {
        eNone = 0,
        eVertex,
        eIndex,
        eStaging
    };

    struct BufferDesc
    {
        BufferType Type;
        uint32_t Size;
        uint32_t Stride;
        uint32_t NumElements;
    };

    enum class PrimitiveTopology
    {
        eNone = 0,
        ePoints,
        eLines,
        eTriangles
    };

    struct PipelineDesc
    {
        std::shared_ptr<Shader> Shader;
        VertexLayout Layout;
        std::shared_ptr<Framebuffer> Framebuffer;
        PrimitiveTopology Topology = PrimitiveTopology::eTriangles;
        bool BackFaceCulling = true;
        bool DepthTest = true;
        bool DepthWrite = false;
        bool Wireframe = false;
        float LineWidth = 1.0f;
    };

}  // namespace gfx

#endif  // PERSONAL_RENDERER_RESOURCEDESCRIPTIONS_H
