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

        auto GetDesc() const -> const TextureDesc& { return m_desc; }

        auto GetImageData() -> RawBuffer& { return m_imageData; }

        auto GetVulkanImage() const -> const vk::Image& { return m_image; }
        auto GetVulkanAllocation() const -> const VmaAllocation& { return m_allocation; }
        auto GetVulkanView() const -> const vk::ImageView& { return m_view; }
        auto GetVulkanSampler() const -> const vk::Sampler& { return m_sampler; }

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
    };
} // namespace gfx
