#pragma once

#include "GFX/Resources/ResourceDescriptions.h"
#include "GFX/Resources/RawBuffer.h"

#include "vk_mem_alloc.h"

#include <vulkan/vulkan.hpp>

#include <string>

#include "VulkanImage.h"

namespace gfx
{
    class DeviceContext;
    class Image;

    class Texture
    {
    public:
        Texture(DeviceContext& deviceCtx, const std::string& path, const TextureDesc& desc);
        Texture(DeviceContext& deviceCtx, const TextureDesc& desc, const void* data);

        auto GetHash() const -> uint64_t;
        auto GetDesc() const -> const TextureDesc& { return m_desc; }

        auto GetImageData() -> RawBuffer& { return m_imageData; }

        auto GetVulkanImage() const -> const vk::Image& { return m_image->GetVulkanImage(); }
        auto GetVulkanAllocation() const -> const VmaAllocation& { return m_image->GetVulkanAllocation(); }
        auto GetVulkanView() const -> const vk::ImageView& { return m_image->GetVulkanView(); }
        auto GetVulkanSampler() const -> const vk::Sampler& { return m_image->GetVulkanSampler(); }
        auto GetVulkanDescriptorInfo() const -> const vk::DescriptorImageInfo& { return m_image->GetVulkanDescriptorInfo(); }

    private:
        void Invalidate();

    private:
        DeviceContext& m_deviceCtx;
        TextureDesc m_desc;

        std::shared_ptr<Image> m_image;

        RawBuffer m_imageData;
    };
}  // namespace gfx
