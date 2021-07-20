#include "VulkanUtils.h"

namespace gfx
{
    auto VkUtils::ToWrapMode(vk::SamplerAddressMode mode) -> WrapMode
    {
        switch (mode)
        {
            default: break;
            case vk::SamplerAddressMode::eRepeat: return WrapMode::eRepeat;
            case vk::SamplerAddressMode::eMirrorClampToEdge: return WrapMode::eMirror;
            case vk::SamplerAddressMode::eClampToEdge: return WrapMode::eClamp;
            case vk::SamplerAddressMode::eClampToBorder: return WrapMode::eBorder;
        }
        return {};
    }

    auto VkUtils::ToVkWrapMode(WrapMode mode) -> vk::SamplerAddressMode
    {
        switch (mode)
        {
            default: break;
            case WrapMode::eRepeat: return vk::SamplerAddressMode::eRepeat;
            case WrapMode::eMirror: return vk::SamplerAddressMode::eMirrorClampToEdge;
            case WrapMode::eClamp: return vk::SamplerAddressMode::eClampToEdge;
            case WrapMode::eBorder: return vk::SamplerAddressMode::eClampToBorder;
        }
        return {};
    }

    auto VkUtils::ToFormat(vk::Format format) -> Format
    {
        switch (format)
        {
            default: break;
            case vk::Format::eR8G8B8A8Srgb: return Format::eRGBA;
        }
        return {};
    }

    auto VkUtils::ToVkFormat(Format format) -> vk::Format
    {
        switch (format)
        {
            default: break;
            case Format::eRGBA: return vk::Format::eR8G8B8A8Srgb;
        }
        return {};
    }

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
