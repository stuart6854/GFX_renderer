#include "GFX/Resources/TextureBuilder.h"

#include "GFX/Debug.h"
#include "GFX/Resources/Texture.h"

#include <algorithm>

namespace gfx
{
    inline auto FloatToByte(const float value) -> uint8_t { return uint8_t(std::clamp(256.0f * value, 0.0f, 255.0f)); }

    TextureBuilder::TextureBuilder(const uint32_t width, const uint32_t height, bool isHdr) : m_width(width), m_height(height)
    {
        if (!isHdr) m_format = TextureFormat::eRGBA;

        m_data.resize(width * height * 4);
    }

    void TextureBuilder::SetPixels(const std::vector<glm::vec4>& colors)
    {
        const auto size = m_width * m_height;

        GFX_ASSERT(colors.size() == size, "Pixel vector must be the same size as texture width * height!");

        for (uint32_t i = 0; i < size; i++)
        {
            const auto& color = colors[i];

            m_data[i * 4] = FloatToByte(color.r);
            m_data[i * 4 + 1] = FloatToByte(color.g);
            m_data[i * 4 + 2] = FloatToByte(color.b);
            m_data[i * 4 + 3] = FloatToByte(color.a);
        }
    }

    void TextureBuilder::SetPixel(uint32_t x, uint32_t y, const glm::vec4& color)
    {
        const auto baseIndex = (x + y * m_width) * 4;

        m_data[baseIndex] = FloatToByte(color.r);
        m_data[baseIndex + 1] = FloatToByte(color.g);
        m_data[baseIndex + 2] = FloatToByte(color.b);
        m_data[baseIndex + 3] = FloatToByte(color.a);
    }

}  // namespace gfx
