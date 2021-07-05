//
// Created by stumi on 11/06/21.
//

#ifndef PERSONAL_RENDERER_RESOURCEDESCRIPTIONS_H
#define PERSONAL_RENDERER_RESOURCEDESCRIPTIONS_H

#include "VertexLayout.h"

#include <glm/vec3.hpp>

#include <cstdint>
#include <memory>

namespace gfxOld
{
    class RenderSurface;
    class Framebuffer;
    class Shader;

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

    enum class FaceCullMode
    {
        eNone = 0,
        eBack,
        eFront,
        eBoth
    };

    struct PipelineDesc
    {
        std::shared_ptr<Shader> Shader;
        VertexLayout Layout;
        std::shared_ptr<Framebuffer> Framebuffer;
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

    enum class ImageFormat
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

    enum class ImageUsage
    {
        eNone = 0,
        eTexture,
        eAttachment,
    };

    struct ImageDesc
    {
        uint32_t Width = 0;
        uint32_t Height = 0;
        uint32_t Layers = 1;
        uint32_t Mips = 1;
        ImageFormat Format = ImageFormat::eRGBA;
        ImageUsage Usage = ImageUsage::eTexture;
    };

    struct TextureDesc
    {
        uint32_t Width = 0;
        uint32_t Height = 0;
        uint32_t Depth = 1;
        uint32_t Layers = 1;
        uint32_t Mips = 1;
        ImageFormat Format;
        ImageUsage Usage = ImageUsage::eTexture;
        // TODO: Sampler Filter
        // TODO: Sampler Wrap
    };

    struct FramebufferAttachmentDesc
    {
        ImageFormat Format;
    };

    struct FramebufferDesc
    {
        uint32_t Width = 0;
        uint32_t Height = 0;
        glm::vec3 ClearColor = { 0.0f, 0.0f, 0.0f };
        bool ClearOnLoad = true;

        std::vector<FramebufferAttachmentDesc> Attachments;
        uint32_t Samples = 1;  // Multisampling

        // SwapChainTarget = screen buffer (no framebuffer)
        bool IsSwapChainTarget = false;
        RenderSurface* SwapChainTarget;
    };

    namespace Utils
    {
        inline bool IsDepthFormat(ImageFormat format)
        {
            if (format == ImageFormat::eDepth32F || format == ImageFormat::eDepth24Stencil8) return true;

            return false;
        }

    }  // namespace Utils

}  // namespace gfxOld

#endif  // PERSONAL_RENDERER_RESOURCEDESCRIPTIONS_H
