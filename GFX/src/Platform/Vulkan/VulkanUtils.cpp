#include "VulkanUtils.h"

namespace gfx
{
    auto VkUtils::ToShaderStage(vk::ShaderStageFlagBits stage) -> ShaderStage
    {
        switch (stage)
        {
            default:
            case vk::ShaderStageFlagBits::eVertex: return ShaderStage::eVertex;
            case vk::ShaderStageFlagBits::eFragment: return ShaderStage::ePixel;
        }
        return ShaderStage::eNone;
    }

    auto VkUtils::ToVkShaderStage(ShaderStage stage) -> vk::ShaderStageFlagBits
    {
        switch (stage)
        {
            default:
            case ShaderStage::eNone: break;
            case ShaderStage::eVertex: return vk::ShaderStageFlagBits::eVertex;
            case ShaderStage::ePixel: return vk::ShaderStageFlagBits::eFragment;
        }
        return {};
    }
}
