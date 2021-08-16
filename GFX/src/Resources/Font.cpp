#include "GFX/Resources/Font.h"

#include "GFX/Debug.h"
#include "GFX/Resources/TextureBuilder.h"
#include "GFX/Resources/Texture.h"
#include "GFX/Utility/TexturePacker.h"

#include <algorithm>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace gfx
{
    namespace Utils
    {
        /*auto ConvertTextureData(const msdfgen::Bitmap<float, 3>& original) -> std::vector<glm::vec4>
        {
            std::vector<glm::vec4> data(original.width() * original.height());

            for (int x = 0; x < original.width(); x++)
            {
                for (int y = 0; y < original.height(); y++)
                {
                    auto* pix = original(x, y);

                    const auto index = x + y * original.width();
                    data[index].r = pix[0];
                    data[index].g = pix[1];
                    data[index].b = pix[2];
                    data[index].a = 1.0f;
                }
            }

            return data;
        }*/
    }

    Font::Font(const std::string& filename)
        : m_filename(filename)
    {
        LoadCharData();
    }

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

        m_fontSize = 48;
        if (FT_Set_Pixel_Sizes(face, 0, m_fontSize))
        {
            GFX_ERROR("Failed to set font pixel size!");
        }

        FT_GlyphSlot glyph = face->glyph;

        m_lineHeight = face->size->metrics.height >> 6;

        std::vector<GlyphTexture> glyphTextures(128);

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

            const uint32_t glyphWidth = glyph->bitmap.width;
            const uint32_t glyphHeight = glyph->bitmap.rows;

            // Convert & store glyph texture data
            glyphTextures[i].Character = i;
            glyphTextures[i].Width = glyphWidth;
            glyphTextures[i].Height = glyphHeight;
            glyphTextures[i].Data.assign(data, data + (glyphWidth * glyphHeight));

            // Store glyph data for later

            auto& glyphData = m_glyphs[i];
            glyphData.Size = { glyphWidth, glyphHeight };
            glyphData.Bearing = { glyph->bitmap_left, glyph->bitmap_top };
            glyphData.UVOrigin = {};
            glyphData.Advance = glyph->advance.x;
            // }
        }
        // Pack glyph textures. Sorted by total pixels (width x height, largest -> smallest) 
        {
            std::ranges::sort(glyphTextures,
                              [](const auto& a, const auto& b)
                              {
                                  if (a.Data.empty()) return false;
                                  if (/* a.Data.empty() && */ b.Data.empty()) return true;

                                  const int aPixels = a.Width * a.Height;
                                  const int bPixels = b.Width * b.Height;
                                  return aPixels > bPixels;
                              });

            // TexturePacker packer(128, 1);
            TexturePacker packer(512, 1);
            for (const auto& glyphTexture : glyphTextures)
            {
                // We can break if a texture is null,
                // because null textures should all be last after sort
                if (glyphTexture.Data.empty()) continue;

                glm::vec2 origin;
                if (!packer.AddToPack(glyphTexture.Data, glyphTexture.Width, glyphTexture.Height, origin))
                {
                    GFX_ERROR("Failed to pack texture! Increase packer texture size!");
                    continue;
                }

                auto& glyph = m_glyphs[glyphTexture.Character];
                glyph.UVOrigin = origin / 512.0f;
                glyph.UVSize = { glyphTexture.Width / 512.0f, glyphTexture.Height / 512.0f };

                // GFX_TRACE("Packed texture: size=({}, {}), pos=({}, {})",
                //           glyphTexture.Width,
                //           glyphTexture.Height,
                //           origin.x,
                //           origin.y);
            }

            m_atlas = packer.CreateTexture();
            packer.WriteToPng("font_atlas.png");
        }

        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    }
}
