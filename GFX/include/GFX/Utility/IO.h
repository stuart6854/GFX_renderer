#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace gfx
{
    void WriteImagePNG(const std::string& filename, uint32_t w, uint32_t h, uint32_t channels, const std::vector<uint8_t>& data);
}
