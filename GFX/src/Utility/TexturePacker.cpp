#include "GFX/Utility/TexturePacker.h"

#include "GFX/Resources/Texture.h"
#include "GFX/Utility/IO.h"

namespace gfx
{
    TexturePacker::TexturePacker(const uint32_t textureSize, const uint32_t channels, const uint32_t spacing)
        : m_textureSize(textureSize),
          m_channels(channels),
          m_spacing(spacing),
          m_x(0),
          m_y(0),
          m_packedData(textureSize * textureSize * 4, 0),
          m_packed(textureSize * textureSize, false)
    {
    }

    bool TexturePacker::AddToPack(const std::vector<uint8_t>& data, const uint32_t width, const uint32_t height, glm::vec2& origin)
    {
        m_pendingTextureWidth = width;
        m_pendingTextureHeight = height;
        m_pendingTextureData = data;

        m_x = 0;
        m_y = 0;

        if (TryAddToPack())
        {
            origin = { m_x, m_y };
            return true;
        }

        return false;
    }

    bool TexturePacker::AddToPack(const TextureBuilder& builder, glm::vec2& origin)
    {
        m_pendingTextureWidth = builder.GetWidth();
        m_pendingTextureHeight = builder.GetHeight();
        m_pendingTextureData = builder.GetData();

        m_x = 0;
        m_y = 0;

        if (TryAddToPack())
        {
            origin = { m_x, m_y };
            return true;
        }

        return false;
    }

    bool TexturePacker::AddToPack(const TextureImporter& importer, glm::vec2& origin)
    {
        m_pendingTextureWidth = importer.GetWidth();
        m_pendingTextureHeight = importer.GetHeight();
        m_pendingTextureData = importer.GetData();

        m_x = 0;
        m_y = 0;

        if (TryAddToPack())
        {
            origin = { m_x, m_y };
            return true;
        }

        return false;
    }

    auto TexturePacker::CreateTexture() const -> OwnedPtr<Texture>
    {
        TextureDesc desc{};
        desc.Width = m_textureSize;
        desc.Height = m_textureSize;
        desc.Usage = TextureUsage::eTexture;
        if (m_channels == 1)
            desc.Format = TextureFormat::eR;
        else if (m_channels == 4)
            desc.Format = TextureFormat::eRGBA;
        return Texture::Create(desc, m_packedData);
    }

    void TexturePacker::WriteToPng(const std::string& filename) const
    {
        WriteImagePNG(filename, m_textureSize, m_textureSize, m_packedData);
    }

    bool TexturePacker::IsPosValidForPendingTexture()
    {
        const int maxWidth = m_pendingTextureWidth + m_spacing;
        const int maxHeight = m_pendingTextureHeight + m_spacing;
        // Early outs
        {
            const auto packedIndex = GetPackedIndex(m_x, m_y);
            if (m_packed[packedIndex] == true)
                return false;
        }
        {
            const auto packedIndex = GetPackedIndex(m_x + maxWidth - 1, m_y);
            if (m_packed[packedIndex] == true)
                return false;
        }
        {
            const auto packedIndex = GetPackedIndex(m_x + maxWidth - 1, m_y + maxHeight - 1);
            if (m_packed[packedIndex] == true)
                return false;
        }
        {
            const auto packedIndex = GetPackedIndex(m_x, m_y + maxHeight - 1);
            if (m_packed[packedIndex] == true)
                return false;
        }

        for (uint32_t y = 0; y < maxHeight; y++)
        {
            for (uint32_t x = 0; x < maxWidth; x++)
            {
                if (m_x + x >= m_textureSize || m_y + y >= m_textureSize)
                    return false;

                const auto packedIndex = GetPackedIndex(m_x + x, m_y + y);

                if (m_packed[packedIndex] == true)
                    return false;
            }
        }
        return true;
    }

    void TexturePacker::AddPendingTextureAtCurrentPosition()
    {
        for (uint32_t y = 0; y < m_pendingTextureHeight + m_spacing; y++)
        {
            for (uint32_t x = 0; x < m_pendingTextureWidth + m_spacing; x++)
            {
                const auto pendingTexIndex = GetPendingDataIndex(x, y);
                const auto packedTexIndex = GetPackedDataIndex(m_x + x, m_y + y);

                if (packedTexIndex >= m_textureSize * m_textureSize * m_channels) continue;

                const auto packedIndex = GetPackedIndex(m_x + x, m_y + y);
                m_packed[packedIndex] = true;

                if (x >= m_pendingTextureWidth) continue;
                if (y >= m_pendingTextureHeight) continue;

                m_packedData[packedTexIndex] = m_pendingTextureData[pendingTexIndex];
                if (m_channels > 1)
                    m_packedData[packedTexIndex + 1] = m_pendingTextureData[pendingTexIndex + 1];
                if (m_channels > 2)
                    m_packedData[packedTexIndex + 2] = m_pendingTextureData[pendingTexIndex + 2];
                if (m_channels > 3)
                    m_packedData[packedTexIndex + 3] = m_pendingTextureData[pendingTexIndex + 3];
            }
        }
    }

    bool TexturePacker::TryAddToPack()
    {
        // Check in scan-line order
        for (m_y = 0; m_y < m_textureSize; m_y++)
        {
            for (m_x = 0; m_x < m_textureSize; m_x++)
            {
                if (IsPosValidForPendingTexture())
                {
                    AddPendingTextureAtCurrentPosition();
                    return true;
                }
            }
        }
        return false;
    }

    auto TexturePacker::GetPackedIndex(const uint32_t x, const uint32_t y) const -> uint32_t
    {
        return x + y * m_textureSize;
    }

    auto TexturePacker::GetPackedDataIndex(const uint32_t x, const uint32_t y) const -> uint32_t
    {
        return (x + y * m_textureSize) * m_channels;
    }

    auto TexturePacker::GetPendingDataIndex(const uint32_t x, const uint32_t y) const -> uint32_t
    {
        return (x + y * m_pendingTextureWidth) * m_channels;
    }
}
