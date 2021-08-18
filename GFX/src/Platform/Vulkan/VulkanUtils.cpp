#include "VulkanUtils.h"

#include "GFX/Debug.h"

namespace gfx
{
    auto VkUtils::ToWrapMode(const vk::SamplerAddressMode mode) -> WrapMode
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

    auto VkUtils::ToVkWrapMode(const WrapMode mode) -> vk::SamplerAddressMode
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

    auto VkUtils::ToTextureFormat(const vk::Format format) -> TextureFormat
    {
        switch (format)
        {
            default: break;
            case vk::Format::eR8G8B8A8Srgb: return TextureFormat::eRGBA;
        }
        return {};
    }

    auto VkUtils::ToVkTextureFormat(const TextureFormat format) -> vk::Format
    {
        // https://stackoverflow.com/questions/59628956/what-is-the-difference-between-normalized-scaled-and-integer-vkformats
        switch (format)
        {
            default: break;
            case TextureFormat::eR: return vk::Format::eR8Unorm;
            case TextureFormat::eRGBA: return vk::Format::eR8G8B8A8Srgb;
            case TextureFormat::eDepth32f: return vk::Format::eD32Sfloat;
            case TextureFormat::eDepth24Stencil8: return vk::Format::eD24UnormS8Uint;
        }
        return {};
    }

    auto VkUtils::ToTextureUsage(vk::ImageUsageFlags) -> TextureUsage
    {
        GFX_WARN("VkUtils::ToTextureUsage() is not implemented!");
        return {};
    }

    auto VkUtils::ToVkTextureUsage(const TextureUsage usage,
                                   const bool isDepthFormat) -> vk::ImageUsageFlags
    {
        vk::ImageUsageFlags imageUsage = vk::ImageUsageFlagBits::eSampled;
        if (usage == TextureUsage::eAttachment)
        {
            if (isDepthFormat)
                imageUsage |= vk::ImageUsageFlagBits::eDepthStencilAttachment;
            else
                imageUsage |= vk::ImageUsageFlagBits::eColorAttachment;
        }
        else if (usage == TextureUsage::eTexture)
        {
            imageUsage |= vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst;
        }
        return imageUsage;
    }

    auto VkUtils::ToShaderStage(const vk::ShaderStageFlagBits stage) -> ShaderStage
    {
        if (stage == vk::ShaderStageFlagBits::eVertex)
            return ShaderStage::eVertex;
        if (stage == vk::ShaderStageFlagBits::eFragment)
            return ShaderStage::ePixel;

        return ShaderStage::eNone;
    }

    auto VkUtils::ToVkShaderStage(const ShaderStage stage) -> vk::ShaderStageFlags
    {
        switch (stage)
        {
            case ShaderStage::eVertex: return vk::ShaderStageFlagBits::eVertex;
            case ShaderStage::ePixel: return vk::ShaderStageFlagBits::eFragment;
            case ShaderStage::eNone:
            default: break;
        }
        return {};
    }
}
