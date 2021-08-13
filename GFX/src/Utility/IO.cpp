#include "GFX/Utility/IO.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

namespace gfx
{
    void WriteImagePNG(const std::string& filename, const uint32_t w, const uint32_t h, const std::vector<uint8_t>& data)
    {
        stbi_write_png(filename.c_str(), w, h, 4, data.data(), w * 4);
    }
}
