#include "GFX/Resources/TextureImporter.h"

#include "GFX/Debug.h"
#include "GFX/Resources/Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include <stb/stb_image.h>

namespace gfx
{
    TextureImporter::TextureImporter(const std::string& filename)
        : m_filename(filename)
    {
        Load(filename);
    }

    void TextureImporter::Load(const std::string& filename)
    {
        int textureWidth = 0;
        int textureHeight = 0;
        int textureChannels = 0;

        m_isHdr = stbi_is_hdr(filename.c_str());

        if (!m_isHdr)
        {
            // Flip texture on load
            stbi_set_flip_vertically_on_load(true);
            // Read texture from file
            uint8_t* data = stbi_load(filename.c_str(), &textureWidth, &textureHeight, &textureChannels, 4);
            if (data == nullptr)
            {
                GFX_ERROR("TextureImporter ({}): {}", filename, stbi_failure_reason());
                return;
            }

            const uint32_t textureSize = textureWidth * textureHeight * 4; // RGBA
            m_data.resize(textureSize);
            std::memcpy(m_data.data(), data, textureSize);

            stbi_image_free(data);

            m_format = TextureFormat::eRGBA;
        }

        m_width = textureWidth;
        m_height = textureHeight;
    }
}
