#include "GFX/Resources/TextureBuilder.h"

#include "GFX/Debug.h"
#include "GFX/Resources/Texture.h"

namespace gfx
{
    inline auto ConvertColorValue(const float value) -> uint8_t
    {
        return static_cast<uint8_t>(std::floor(value >= 1.0f ? 255 : value * 256.0f));
    }

    TextureBuilder::TextureBuilder(const uint32_t width, const uint32_t height, bool isHdr)
        : m_width(width),
          m_height(height)
    {
        if(!isHdr) m_format = TextureFormat::eRGBA;

        m_data.resize(width * height * 4);
    }

    void TextureBuilder::SetPixels(const std::vector<glm::vec4>& colors)
    {
        const auto size = m_width * m_height;

        GFX_ASSERT(colors.size() == size, "Pixel vector must be the same size as texture width * height!");

        for (uint32_t i = 0; i < size; i++)
        {
            const auto& color = colors[i];

            m_data[i * 4] = ConvertColorValue(color.r);
            m_data[i * 4 + 1] = ConvertColorValue(color.g);
            m_data[i * 4 + 2] = ConvertColorValue(color.b);
            m_data[i * 4 + 3] = ConvertColorValue(color.a);
        }
    }

    void TextureBuilder::SetPixel(uint32_t x, uint32_t y, const glm::vec4& color)
    {
        const auto baseIndex = (x + y * m_width) * 4;

        m_data[baseIndex] = ConvertColorValue(color.r);
        m_data[baseIndex + 1] = ConvertColorValue(color.g);
        m_data[baseIndex + 2] = ConvertColorValue(color.b);
        m_data[baseIndex + 3] = ConvertColorValue(color.a);
    }
}
