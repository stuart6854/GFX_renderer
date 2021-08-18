#include "GFX/Resources/Font.h"

#include "GFX/Debug.h"
#include "GFX/Resources/TextureBuilder.h"
#include "GFX/Resources/Texture.h"
#include "GFX/Utility/RectPacker.h"
#include "Utility/Timer.h"
#include "GFX/Utility/IO.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <algorithm>
#include <random>

namespace gfx
{
    Font::Font(const std::string& filename) : m_filename(filename) { LoadCharData(); }

    auto Font::GetGlyph(const char character) -> const FontGlyph&
    {
        GFX_ASSERT(character > char(128), "Only compatible with ASCII characters!");

        return m_glyphs.at(character);
    }

    void Font::LoadCharData()
    {
        FT_Library ft;
        if (FT_Init_FreeType(&ft))
        {
            GFX_ERROR("Failed to initialise FreeType!");
            return;
        }

        FT_Face face;
        if (FT_New_Face(ft, m_filename.c_str(), 0, &face))
        {
            GFX_ERROR("Failed to load font! ({})", m_filename);
            return;
        }

        m_fontSize = 32;
        if (FT_Set_Pixel_Sizes(face, 0, m_fontSize))
        {
            GFX_ERROR("Failed to set font pixel size!");
        }

        FT_GlyphSlot glyph = face->glyph;

        m_lineHeight = face->size->metrics.height >> 6;

        std::vector<GlyphTexture> glyphTextures(128);

        const int atlasWidth = 512;
        const int atlasHeight = 512;
        RectPacker rectPacker(atlasWidth, atlasHeight);

        {
            GFX_SCOPED_TIMER("Generate Font Glyphs");

            // for (int i = 65; i < 68; i++)
            for (int i = 0; i < 128; i++)
            {
                auto error = FT_Load_Char(face, i, FT_LOAD_DEFAULT);
                if (error)
                {
                    GFX_ERROR("Error loading glyph! ('{}')", char(i));
                    continue;
                }

                error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_SDF);
                if (error)
                {
                    GFX_ERROR("Failed to render glyph! ('{}')", char(i));
                    continue;
                }

                uint8_t* data = glyph->bitmap.buffer;

                const int glyphWidth = glyph->bitmap.width;
                const int glyphHeight = glyph->bitmap.rows;

                // Convert & store glyph texture data
                glyphTextures[i].Character = i;
                glyphTextures[i].Width = glyphWidth;
                glyphTextures[i].Height = glyphHeight;
                glyphTextures[i].Data.assign(data, data + (glyphWidth * glyphHeight));

                if (glyphWidth > 0 && glyphHeight > 0) rectPacker.AddRect(i, glyphWidth, glyphHeight);

                // Store glyph data for later

                auto& glyphData = m_glyphs[i];
                glyphData.Size = { glyphWidth, glyphHeight };
                glyphData.Bearing = { glyph->bitmap_left, glyph->bitmap_top };
                glyphData.UVOrigin = {};
                glyphData.Advance = glyph->advance.x;
            }
        }
        {
            GFX_SCOPED_TIMER("Font::PackGlyphs");
            if (!rectPacker.Pack()) GFX_ERROR("Failed to pack rects!");
        }
        {
            GFX_SCOPED_TIMER("Font::CreateAtlas");
            std::vector<uint8_t> atlasData(atlasWidth * atlasHeight);

            for (const auto& rect : rectPacker.GetAllPackedRects())
            {
                auto& glyphTexture = glyphTextures[rect.id];
                auto& glyphData = m_glyphs[rect.id];

                glyphData.UVOrigin = { (float)rect.x / (float)atlasWidth, (float)rect.y / (float)atlasHeight };
                glyphData.UVSize = { (float)rect.width / (float)atlasWidth, (float)rect.height / (float)atlasHeight };

                int i = 0;
                for (int y = rect.y; y < rect.y + rect.height; y++)
                {
                    for (int x = rect.x; x < rect.x + rect.width; x++)
                    {
                        atlasData[x + y * atlasWidth] = glyphTexture.Data[i];
                        i++;
                    }
                }
            }

            TextureDesc desc{};
            desc.Width = atlasWidth;
            desc.Height = atlasHeight;
            desc.Usage = TextureUsage::eTexture;
            desc.Format = TextureFormat::eR;
            m_atlas = Texture::Create(desc, atlasData);

            // WriteImagePNG("font_atlas.png", atlasWidth, atlasHeight, 1, atlasData);
        }

        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    }

}  // namespace gfx
