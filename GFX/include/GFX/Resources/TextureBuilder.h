#pragma once

#include <glm/vec4.hpp>

#include <cstdint>
#include <vector>

namespace gfx
{
    enum class TextureFormat;

    class TextureBuilder
    {
    public:
        TextureBuilder(uint32_t width, uint32_t height, bool isHdr = false);

        auto GetWidth() const -> uint32_t { return m_width; }
        auto GetHeight() const -> uint32_t { return m_height; }
        auto GetFormat() const -> TextureFormat { return m_format; }

        auto GetData() const -> const std::vector<uint8_t>& { return m_data; }

        void SetPixels(const std::vector<glm::vec4>& colors);
        void SetPixel(uint32_t x, uint32_t y, const glm::vec4& color);

    private:
        uint32_t m_width = 0;
        uint32_t m_height = 0;
        TextureFormat m_format;

        std::vector<uint8_t> m_data = {};
    };
}  // namespace gfx
