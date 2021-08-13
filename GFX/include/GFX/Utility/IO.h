#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace gfx
{
    void WriteImagePNG(const std::string& filename, uint32_t w, uint32_t h, const std::vector<uint8_t>& data);
}
