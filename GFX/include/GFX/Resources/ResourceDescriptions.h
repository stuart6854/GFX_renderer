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
        eUniformBuffer,
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
        bool DepthWrite = true;
        bool Wireframe = false;
        float LineWidth = 1.0f;
    };

    enum class TextureFormat
    {
        eNone = 0,
        eRGB,
        eRGBA,
        eRGBA16F,
        eRGBA32F,
        eRG32F,

        eSRGB,

        eDepth32F,
        eDepth24Stencil8,

        // Default
        eDepth = eDepth24Stencil8
    };

    enum class TextureUsage
    {
        eNone = 0,
        eTexture,
        eAttachment,
    };

    struct TextureDesc
    {
        uint32_t Width = 0;
        uint32_t Height = 0;
        uint32_t Depth = 1;
        uint32_t Layers = 1;
        uint32_t Mips = 1;
        TextureFormat Format;
        TextureUsage Usage = TextureUsage::eTexture;
        // TODO: Sampler Filter
        // TODO: Sampler Wrap
    };

    namespace Utils
    {
        inline bool IsDepthFormat(TextureFormat format)
        {
            if (format == TextureFormat::eDepth32F || format == TextureFormat::eDepth24Stencil8) return true;

            return false;
        }

    }  // namespace Utils

}  // namespace gfx

#endif  // PERSONAL_RENDERER_RESOURCEDESCRIPTIONS_H
