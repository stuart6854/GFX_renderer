#pragma once

#include "GFX/Resources/Shader.h"

#include <vulkan/vulkan.hpp>

namespace gfx
{
    namespace VkUtils
    {
        auto ToShaderStage(vk::ShaderStageFlagBits stage) -> ShaderStage;
        auto ToVkShaderStage(ShaderStage stage) -> vk::ShaderStageFlagBits;
    }
}
