#pragma once

#include "GFX/Core/Base.h"
#include "TextureBuilder.h"
#include "TextureImporter.h"

namespace gfx
{
    enum class WrapMode
    {
        eRepeat = 0,
        eMirror,
        eClamp,
        eBorder
    };

    enum class FilterMode
    {
        eNearest,
        eLinear
    };

    enum class TextureFormat
    {
        eNone,
        eRGBA,

        eDepth32f,
        eDepth24Stencil8,

        // default depth
        eDepth = eDepth24Stencil8
    };

    enum class TextureUsage
    {
        eNone = 0,
        eTexture,
        eAttachment
    };

    struct TextureDesc
    {
        uint32_t Width = 0;
        uint32_t Height = 0;
        uint32_t Depth = 1;
        uint32_t Layers = 1;
        uint32_t Mips = 1;
        TextureFormat Format;
        TextureUsage Usage;
    };

    class Texture
    {
    public:
        static auto Create(const TextureBuilder& builder) -> OwnedPtr<Texture>;
        static auto Create(const TextureImporter& importer) -> OwnedPtr<Texture>;
        static auto Create(const TextureDesc& desc) -> OwnedPtr<Texture>;

        virtual ~Texture() = default;

        virtual auto GetWidth() const -> uint32_t = 0;
        virtual auto GetHeight() const -> uint32_t = 0;
    };

    inline bool IsDepthFormat(TextureFormat format)
    {
        return format == TextureFormat::eDepth32f || format == TextureFormat::eDepth24Stencil8;
    }
}
