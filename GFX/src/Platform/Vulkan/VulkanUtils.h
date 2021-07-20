#pragma once

#include "GFX/Resources/Texture.h"
#include "GFX/Resources/Shader.h"
#include "GFX/Resources/CommandBuffer.h"

#include <vulkan/vulkan.hpp>

namespace gfx
{
    namespace VkUtils
    {
        auto ToWrapMode(vk::SamplerAddressMode mode) -> WrapMode;
        auto ToVkWrapMode(WrapMode mode) -> vk::SamplerAddressMode;

        auto ToFormat(vk::Format format) -> Format;
        auto ToVkFormat(Format format) -> vk::Format;

        auto ToShaderStage(vk::ShaderStageFlagBits stage) -> ShaderStage;
        auto ToVkShaderStage(ShaderStage stage) -> vk::ShaderStageFlagBits;
    }
}
