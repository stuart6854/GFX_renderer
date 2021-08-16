#pragma once

#include "GFX/Core/Base.h"

#include <glm/vec2.hpp>

#include <cstdint>
#include <array>
#include <string>
#include <vector>

namespace gfx
{
    class Texture;
    class TextureBuilder;

    struct FontGlyph
    {
        glm::vec2 Size;     // Size of glyph
        glm::vec2 Bearing;  // Offset from baseline to left/top of glyph
        glm::vec2 UVOrigin; // Origin of the texture glyph
        glm::vec2 UVSize;   // Size of the texture glyph
        uint32_t Advance;   // Horizontal offset to advance to next glyph
    };

    class Font
    {
    public:
        Font(const std::string& filename);

        auto GetFontSize() const -> uint32_t { return m_fontSize; }

        auto GetLineHeight() const -> uint32_t { return m_lineHeight; }

        auto GetGlyph(char character) -> const FontGlyph&;

        auto GetAtlasTexture() const -> SharedPtr<Texture> { return m_atlas; }

    private:
        struct GlyphTexture
        {
            uint32_t Character = 0;
            uint32_t Width = 0;
            uint32_t Height = 0;
            std::vector<uint8_t> Data;
        };

        void LoadCharData();
        void GenerateAtlas();

    private:
        std::string m_filename;

        uint32_t m_fontSize = 0;
        uint32_t m_lineHeight = 0;
        uint32_t m_maxHeight = 0;
        std::array<FontGlyph, 128> m_glyphs;

        SharedPtr<gfx::Texture> m_atlas;
    };
}
