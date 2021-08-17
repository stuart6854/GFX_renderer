#include "GFX/Utility/IO.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

namespace gfx
{
    void WriteImagePNG(const std::string& filename, uint32_t w, uint32_t h, uint32_t channels, const std::vector<uint8_t>& data)
    {
        stbi_write_png(filename.c_str(), w, h, channels, data.data(), w * channels);
    }
}
