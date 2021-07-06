#pragma once

#include <string>
#include <vector>

namespace gfx
{
    class TextureImporter
    {
    public:
        TextureImporter(const std::string& filename);

        auto GetFilename() const -> const std::string& { return m_filename; }

        auto GetWidth() const -> uint32_t { return m_width; }
        auto GetHeight() const -> uint32_t { return m_height; }
        auto IsHDR() const -> bool { return m_isHDR; }

        auto GetData() const -> const std::vector<uint8_t>& { return m_data; }

    private:
        void Load(const std::string& filename);

    private:
        std::string m_filename = "";

        uint32_t m_width = 0;
        uint32_t m_height = 0;
        bool m_isHDR = false;

        std::vector<uint8_t> m_data = {};
    };
}
