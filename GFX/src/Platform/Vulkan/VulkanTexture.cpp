#include "VulkanTexture.h"

#include "GFX/Debug.h"

#include "VulkanBackend.h"
#include "VulkanAllocator.h"
#include "VulkanBuffer.h"
#include "VulkanUtils.h"

namespace gfx
{
    VulkanTexture::VulkanTexture(const TextureImporter& importer)
    {
        TextureDesc desc{};
        desc.Width = importer.GetWidth();
        desc.Height = importer.GetHeight();
        desc.Format = importer.GetFormat();
        desc.Usage = TextureUsage::eTexture;
        Init(desc);

        SetData(importer.GetData());
    }

    VulkanTexture::VulkanTexture(const TextureBuilder& builder)
    {
        TextureDesc desc{};
        desc.Width = builder.GetWidth();
        desc.Height = builder.GetHeight();
        desc.Format = builder.GetFormat();
        desc.Usage = TextureUsage::eTexture;
        Init(desc);

        SetData(builder.GetData());
    }

    VulkanTexture::VulkanTexture(const TextureDesc& desc)
    {
        Init(desc);
    }

    VulkanTexture::~VulkanTexture()
    {
        auto* backend = VulkanBackend::Get();
        auto& allocator = backend->GetAllocator();
        auto vkDevice = backend->GetDevice().GetHandle();

        vkDevice.destroy(m_sampler);
        vkDevice.destroy(m_view);

        allocator.Free(m_image, m_allocation);
    }

    void VulkanTexture::Init(const TextureDesc& desc)
    {
        m_width = desc.Width;
        m_height = desc.Height;
        m_format = desc.Format;

        auto* backend = VulkanBackend::Get();
        auto& allocator = backend->GetAllocator();
        auto vkDevice = backend->GetDevice().GetHandle();

        const bool isDepthFormat = IsDepthFormat(desc.Format);
        const auto usage = VkUtils::ToVkTextureUsage(desc.Usage, isDepthFormat);
        vk::ImageAspectFlags aspectMask = isDepthFormat ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor;
        if (desc.Format == TextureFormat::eDepth24Stencil8) aspectMask |= vk::ImageAspectFlagBits::eStencil;

        vk::ImageCreateInfo imageInfo{};
        imageInfo.setImageType(vk::ImageType::e2D);
        imageInfo.setFormat(VkUtils::ToVkTextureFormat(desc.Format));
        imageInfo.extent.setWidth(m_width);
        imageInfo.extent.setHeight(m_height);
        imageInfo.extent.setDepth(1);
        imageInfo.setMipLevels(1);
        imageInfo.setArrayLayers(1);
        imageInfo.setUsage(usage);
        imageInfo.setInitialLayout(vk::ImageLayout::eUndefined);
        imageInfo.setTiling(vk::ImageTiling::eOptimal);
        imageInfo.setSamples(vk::SampleCountFlagBits::e1);

        allocator.Allocate(imageInfo, VMA_MEMORY_USAGE_GPU_ONLY, &m_image, &m_allocation);

        vk::ImageViewCreateInfo viewInfo{};
        viewInfo.setFormat(VkUtils::ToVkTextureFormat(desc.Format));
        viewInfo.setImage(m_image);
        viewInfo.setViewType(vk::ImageViewType::e2D);
        viewInfo.subresourceRange.setAspectMask(aspectMask);
        viewInfo.subresourceRange.setBaseMipLevel(0);
        viewInfo.subresourceRange.setLevelCount(1);
        viewInfo.subresourceRange.setLayerCount(1);
        viewInfo.subresourceRange.setBaseArrayLayer(0);

        m_view = vkDevice.createImageView(viewInfo);

        vk::SamplerCreateInfo samplerInfo{};
        samplerInfo.setAddressModeU(vk::SamplerAddressMode::eRepeat);
        samplerInfo.setAddressModeV(vk::SamplerAddressMode::eRepeat);
        samplerInfo.setAddressModeW(vk::SamplerAddressMode::eRepeat);
        // samplerInfo.borderColor = vk::BorderColor::
        samplerInfo.setMagFilter(vk::Filter::eNearest);
        samplerInfo.setMinFilter(vk::Filter::eNearest);

        m_sampler = vkDevice.createSampler(samplerInfo);
    }

    void VulkanTexture::SetData(const std::vector<uint8_t>& data) const
    {
        auto* backend = VulkanBackend::Get();

        const auto stagingBuffer = Buffer::CreateStaging(data.size(), data.data());
        auto* vkStagingBuffer = static_cast<VulkanBuffer*>(stagingBuffer.get());

        auto& device = backend->GetDevice();
        auto cmdBuffer = device.GetCommandBuffer(true);

        TransitionImageLayout(cmdBuffer, m_image, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);

        vk::BufferImageCopy copyRegion{};
        copyRegion.setBufferOffset(0);
        copyRegion.setBufferRowLength(0);
        copyRegion.setBufferImageHeight(0);
        copyRegion.imageSubresource.setAspectMask(vk::ImageAspectFlagBits::eColor);
        copyRegion.imageSubresource.setMipLevel(0);
        copyRegion.imageSubresource.setBaseArrayLayer(0);
        copyRegion.imageSubresource.setLayerCount(1);
        copyRegion.imageOffset = vk::Offset3D(0, 0, 0);
        copyRegion.imageExtent = vk::Extent3D(m_width, m_height, 1);

        cmdBuffer.copyBufferToImage(vkStagingBuffer->GetHandle(), m_image, vk::ImageLayout::eTransferDstOptimal, copyRegion);

        TransitionImageLayout(cmdBuffer, m_image, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

        device.FlushCommandBuffer(cmdBuffer);
    }

    void VulkanTexture::TransitionImageLayout(vk::CommandBuffer& cmdBuffer, vk::Image image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout) const
    {
        vk::ImageMemoryBarrier barrier{};
        barrier.setImage(image);
        barrier.setOldLayout(oldLayout);
        barrier.setNewLayout(newLayout);
        barrier.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
        barrier.subresourceRange.setBaseMipLevel(0);
        barrier.subresourceRange.setLevelCount(1);
        barrier.subresourceRange.setBaseArrayLayer(0);
        barrier.subresourceRange.setLayerCount(1);

        vk::PipelineStageFlags srcStage;
        vk::PipelineStageFlags dstStage;
        if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
        {
            barrier.setSrcAccessMask({});
            barrier.setDstAccessMask(vk::AccessFlagBits::eTransferWrite);

            srcStage = vk::PipelineStageFlagBits::eTopOfPipe;
            dstStage = vk::PipelineStageFlagBits::eTransfer;
        }
        else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
        {
            barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite);
            barrier.setDstAccessMask(vk::AccessFlagBits::eShaderRead);

            srcStage = vk::PipelineStageFlagBits::eTransfer;
            dstStage = vk::PipelineStageFlagBits::eFragmentShader;
        }
        else
        {
            GFX_ERROR("Unsupported layout transition!");
        }

        cmdBuffer.pipelineBarrier(srcStage, dstStage, {}, {}, {}, barrier);
    }
}
