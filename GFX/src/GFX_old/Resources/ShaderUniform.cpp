//
// Created by stumi on 16/06/21.
//
#include "GFX/Resources/ShaderUniform.h"

namespace gfxOld
{
    auto ShaderUniform::UniformTypeToString(ShaderUniformType type) -> std::string
    {
        if (type == ShaderUniformType::eBool) return "Boolean";
        if (type == ShaderUniformType::eInt) return "Int";
        if (type == ShaderUniformType::eFloat) return "Float";
        return "None";
    }

}  // namespace gfxOld