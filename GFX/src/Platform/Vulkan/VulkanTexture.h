#pragma once

#include "GFX/Resources/ResourceDescriptions.h"
#include "GFX/Resources/RawBuffer.h"

#include "vk_mem_alloc.h"

#include <string>

namespace gfx
{
    class DeviceContext;

    class Texture
    {
    public:
        Texture(DeviceContext& deviceCtx, const std::string& path, const TextureDesc& desc);
        Texture(DeviceContext& deviceCtx, const TextureDesc& desc, const void* data);

        auto GetHash() const -> uint64_t;
        auto GetDesc() const -> const TextureDesc& { return m_desc; }

        auto GetImageData() -> RawBuffer& { return m_imageData; }

        auto GetVulkanImage() const -> const vk::Image& { return m_image; }
        auto GetVulkanAllocation() const -> const VmaAllocation& { return m_allocation; }
        auto GetVulkanView() const -> const vk::ImageView& { return m_view; }
        auto GetVulkanSampler() const -> const vk::Sampler& { return m_sampler; }
        auto GetVulkanDescriptorInfo() const -> const vk::DescriptorImageInfo& { return m_descriptorImageInfo; }

    private:
        void Invalidate();

    private:
        DeviceContext& m_deviceCtx;
        TextureDesc m_desc;

        vk::Image m_image;
        VmaAllocation m_allocation;
        vk::ImageView m_view;
        vk::Sampler m_sampler;

        RawBuffer m_imageData;

        vk::DescriptorImageInfo m_descriptorImageInfo;
    };
}  // namespace gfx
