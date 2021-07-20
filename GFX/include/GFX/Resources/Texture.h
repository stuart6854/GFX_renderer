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

    enum class Format
    {
        eRGBA
    };

    class Texture
    {
    public:
        static auto Create(const TextureBuilder& builder) -> OwnedPtr<Texture>;
        static auto Create(const TextureImporter& importer) -> OwnedPtr<Texture>;

        virtual ~Texture() = default;

        virtual auto GetWidth() const -> uint32_t = 0;
        virtual auto GetHeight() const -> uint32_t = 0;
    };
}
