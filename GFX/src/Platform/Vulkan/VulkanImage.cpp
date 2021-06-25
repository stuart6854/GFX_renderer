#ifdef GFX_API_VULKAN

    #include "VulkanImage.h"

    #include "VulkanCore.h"

namespace gfx
{
    Image::Image(const ImageDesc& desc) : m_desc(desc) {}

    void Image::Invalidate()
    {
        const auto device = Vulkan::GetDevice();
        const auto allocator = Vulkan::GetAllocator();

        vk::ImageUsageFlags usage = vk::ImageUsageFlagBits::eSampled;
        if (m_desc.Usage == ImageUsage::eAttachment)
        {
            if (Utils::IsDepthFormat(m_desc.Format))
                usage |= vk::ImageUsageFlagBits::eDepthStencilAttachment;
            else
                usage |= vk::ImageUsageFlagBits::eColorAttachment;
        }
        else if (m_desc.Usage == ImageUsage::eTexture)
        {
            usage |= vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst;
        }

        const auto vulkanFormat = Utils::VulkanTextureFormat(m_desc.Format);

        vk::ImageCreateInfo imageInfo{};
        imageInfo.setImageType(vk::ImageType::e2D);
        imageInfo.setFormat(vulkanFormat);
        imageInfo.extent.setWidth(m_desc.Width);
        imageInfo.extent.setHeight(m_desc.Height);
        imageInfo.extent.setDepth(1);
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
        if (m_desc.Format == ImageFormat::eDepth24Stencil8) aspectMask |= vk::ImageAspectFlagBits::eStencil;

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

        UpdateDescriptor();
    }

    void Image::Release()
    {
        const auto device = Vulkan::GetDevice();
        const auto allocator = Vulkan::GetAllocator();

        device.destroy(m_sampler);
        device.destroy(m_view);

        allocator.Free(m_image, m_allocation);
    }

    void Image::UpdateDescriptor()
    {
        if (m_desc.Format == ImageFormat::eDepth24Stencil8 || m_desc.Format == ImageFormat::eDepth32F)
            m_descriptorImageInfo.setImageLayout(vk::ImageLayout::eDepthStencilReadOnlyOptimal);
        else
            m_descriptorImageInfo.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

        m_descriptorImageInfo.setImageView(m_view);
        m_descriptorImageInfo.setSampler(m_sampler);
    }

    namespace Utils
    {
        auto VulkanTextureFormat(const ImageFormat format) -> vk::Format
        {
            switch (format)
            {
                case ImageFormat::eRGB: return vk::Format::eR8G8B8Unorm;
                case ImageFormat::eRGBA: return vk::Format::eR8G8B8A8Unorm;
                case ImageFormat::eDepth32F: return vk::Format::eD32Sfloat;
                case ImageFormat::eDepth24Stencil8: return vk::Format::eD24UnormS8Uint;
                default: break;
            }
            GFX_ASSERT(false);
            return vk::Format::eUndefined;
        }

    }  // namespace Utils

}  // namespace gfx

#endif
