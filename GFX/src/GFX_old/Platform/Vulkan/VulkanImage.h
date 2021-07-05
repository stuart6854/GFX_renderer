#pragma once

#include "GFX/Resources/ResourceDescriptions.h"
#include "GFX/Resources/RawBuffer.h"

#include "vk_mem_alloc.h"

#include <vulkan/vulkan.hpp>

namespace gfxOld
{
    class Image
    {
    public:
        Image(const ImageDesc& desc);
        ~Image();

        auto GetHash() const -> uint64_t { return reinterpret_cast<uint64_t>(static_cast<VkImage>(m_image)); }

        auto GetDesc() -> ImageDesc& { return m_desc; }

        auto GetImageData() -> RawBuffer& { return m_imageData; }

        auto GetVulkanImage() const -> const vk::Image& { return m_image; }
        auto GetVulkanAllocation() const -> const VmaAllocation& { return m_allocation; }
        auto GetVulkanView() const -> const vk::ImageView& { return m_view; }
        auto GetVulkanSampler() const -> const vk::Sampler& { return m_sampler; }
        auto GetVulkanDescriptorInfo() const -> const vk::DescriptorImageInfo& { return m_descriptorImageInfo; }

        void Invalidate();
        void Release();

    private:
        void UpdateDescriptor();

    private:
        ImageDesc m_desc;

        vk::Image m_image;
        vk::ImageView m_view;
        vk::Sampler m_sampler;
        VmaAllocation m_allocation;

        RawBuffer m_imageData;

        vk::DescriptorImageInfo m_descriptorImageInfo;
    };

    namespace Utils
    {
        auto VulkanTextureFormat(const ImageFormat format) -> vk::Format;

    }  // namespace Utils

}  // namespace gfxOld
