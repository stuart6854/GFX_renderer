#pragma once

#include "GFX/Core/Base.h"

#include <string>

namespace gfx
{
    class Shader
    {
    public:
        static auto Create(const std::string& vertexSource, const std::string& pixelSource) -> OwnedPtr<Shader>;

        virtual ~Shader() = default;
    };
}
