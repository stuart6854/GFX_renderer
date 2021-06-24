#include "VulkanTexture.h"

#include "VulkanCore.h"

#include "GFX/Debug.h"
#include "GFX/DeviceContext.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace gfx
{
    namespace Utils
    {
        auto VulkanTextureFormat(TextureFormat format) -> vk::Format
        {
            switch (format)
            {
                case TextureFormat::eRGB: return vk::Format::eR8G8B8Unorm;
                case TextureFormat::eRGBA: return vk::Format::eR8G8B8A8Unorm;
                case TextureFormat::eDepth32F: return vk::Format::eD32Sfloat;
                case TextureFormat::eDepth24Stencil8: return vk::Format::eD24UnormS8Uint;
                default: break;
            }
            GFX_ASSERT(false);
            return vk::Format::eUndefined;
        }
    }  // namespace Utils

    Texture::Texture(DeviceContext& deviceCtx, const std::string& path, const TextureDesc& desc) : m_deviceCtx(deviceCtx), m_desc(desc)
    {
        int width, height, channels;

        if (stbi_is_hdr(path.c_str()))
        {
            m_imageData.Data = (uint8_t*)stbi_loadf(path.c_str(), &width, &height, &channels, 4);
            m_imageData.Size = width * height * 4 * sizeof(float);
            m_desc.Format = TextureFormat::eRGBA32F;
        }
        else
        {
            stbi_set_flip_vertically_on_load(1);
            m_imageData.Data = stbi_load(path.c_str(), &width, &height, &channels, 4);
            m_imageData.Size = width * height * 4;
            m_desc.Format = TextureFormat::eRGBA;
        }

        GFX_ASSERT(m_imageData.Data, "Failed to load image!");

        m_desc.Width = width;
        m_desc.Height = height;
        m_desc.Depth = 1;
        m_desc.Layers = 1;

        GFX_ASSERT(m_desc.Format != TextureFormat::eNone);

        Invalidate();
    }

    Texture::Texture(DeviceContext& deviceCtx, const TextureDesc& desc, const void* data) : m_deviceCtx(deviceCtx), m_desc(desc)
    {
        GFX_ASSERT(m_desc.Format == TextureFormat::eRGBA);

        const uint32_t size = m_desc.Width * m_desc.Height * 4;
        m_imageData = RawBuffer::Copy(data, size);

        Invalidate();
    }

    auto Texture::GetHash() const -> uint64_t { return reinterpret_cast<uint64_t>(static_cast<VkImage>(m_image)); }

    void Texture::Invalidate()
    {
        const auto device = Vulkan::GetDevice();
        const auto allocator = Vulkan::GetAllocator();

        vk::ImageUsageFlags usage = vk::ImageUsageFlagBits::eSampled;
        if (m_desc.Usage == TextureUsage::eAttachment)
        {
            if (Utils::IsDepthFormat(m_desc.Format))
                usage |= vk::ImageUsageFlagBits::eDepthStencilAttachment;
            else
                usage |= vk::ImageUsageFlagBits::eColorAttachment;
        }
        else if (m_desc.Usage == TextureUsage::eTexture)
        {
            usage |= vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst;
        }

        auto vulkanFormat = Utils::VulkanTextureFormat(m_desc.Format);

        vk::ImageCreateInfo imageInfo{};
        imageInfo.setImageType(vk::ImageType::e2D);
        imageInfo.setFormat(vulkanFormat);
        imageInfo.extent.setWidth(m_desc.Width);
        imageInfo.extent.setHeight(m_desc.Height);
        imageInfo.extent.setDepth(m_desc.Depth);
        imageInfo.setMipLevels(m_desc.Mips);
        imageInfo.setArrayLayers(m_desc.Layers);
        imageInfo.setUsage(usage);
        imageInfo.setSamples(vk::SampleCountFlagBits::e1);
        imageInfo.setTiling(vk::ImageTiling::eOptimal);

        allocator.Allocate(imageInfo, VMA_MEMORY_USAGE_GPU_ONLY, &m_image, &m_allocation);
        GFX_ASSERT(m_image);
        GFX_ASSERT(m_allocation);

        // Image View
        vk::ImageAspectFlags aspectMask = Utils::IsDepthFormat(m_desc.Format) ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor;
        if (m_desc.Format == TextureFormat::eDepth24Stencil8) aspectMask |= vk::ImageAspectFlagBits::eStencil;

        vk::ImageViewCreateInfo viewInfo;
        viewInfo.setFormat(vulkanFormat);
        viewInfo.setViewType(m_desc.Layers > 1 ? vk::ImageViewType::e2DArray : vk::ImageViewType::e2D);
        viewInfo.setComponents({ vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA });
        viewInfo.subresourceRange.setAspectMask(aspectMask);
        viewInfo.subresourceRange.setBaseMipLevel(0);
        viewInfo.subresourceRange.setLevelCount(m_desc.Mips);
        viewInfo.subresourceRange.setBaseArrayLayer(0);
        viewInfo.subresourceRange.setLayerCount(m_desc.Layers);
        viewInfo.setImage(m_image);

        m_view = device.createImageView(viewInfo);

        vk::SamplerCreateInfo samplerInfo{};
        samplerInfo.setMagFilter(vk::Filter::eLinear);
        samplerInfo.setMinFilter(vk::Filter::eLinear);
        samplerInfo.setMipmapMode(vk::SamplerMipmapMode::eLinear);
        samplerInfo.setAddressModeU(vk::SamplerAddressMode::eClampToEdge);
        samplerInfo.setAddressModeV(samplerInfo.addressModeU);
        samplerInfo.setAddressModeW(samplerInfo.addressModeU);
        samplerInfo.setMipLodBias(0.0f);
        samplerInfo.setMaxAnisotropy(1.0f);
        samplerInfo.setMinLod(0.0f);
        samplerInfo.setMaxLod(1.0f);
        samplerInfo.setBorderColor(vk::BorderColor::eFloatOpaqueWhite);

        m_sampler = device.createSampler(samplerInfo);

        m_deviceCtx.Upload(this);

        /* Update descriptor image info */
        if (m_desc.Format == TextureFormat::eDepth24Stencil8 || m_desc.Format == TextureFormat::eDepth32F)
            m_descriptorImageInfo.setImageLayout(vk::ImageLayout::eDepthStencilReadOnlyOptimal);
        else
            m_descriptorImageInfo.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

        m_descriptorImageInfo.setImageView(m_view);
        m_descriptorImageInfo.setSampler(m_sampler);
    }

}  // namespace gfx
