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

        auto ToTextureFormat(vk::Format format) -> TextureFormat;
        auto ToVkTextureFormat(TextureFormat format) -> vk::Format;

        auto ToTextureUsage(vk::ImageUsageFlags usage) -> TextureUsage;
        auto ToVkTextureUsage(TextureUsage usage, bool isDepthFormat) -> vk::ImageUsageFlags;

        auto ToShaderStage(vk::ShaderStageFlagBits stage) -> ShaderStage;
        auto ToVkShaderStage(ShaderStage stage) -> vk::ShaderStageFlags;
    }
}
