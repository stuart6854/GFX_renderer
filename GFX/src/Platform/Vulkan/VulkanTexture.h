﻿#pragma once

#include "GFX/Resources/Texture.h"
#include "vk_mem_alloc.h"

#include <vulkan/vulkan.hpp>

namespace gfx
{
    class VulkanTexture : public Texture
    {
    public:
        VulkanTexture(const TextureImporter& importer);
        VulkanTexture(const TextureBuilder& builder);
        ~VulkanTexture() override;

        auto GetWidth() const -> uint32_t override { return m_width; }
        auto GetHeight() const -> uint32_t override { return m_height; }

        auto GetHandle() const -> vk::Image { return m_image; }
        auto GetView() const -> vk::ImageView { return m_view; }
        auto GetSampler() const -> vk::Sampler { return m_sampler; }

    private:
        void TransitionImageLayout(vk::CommandBuffer& cmdBuffer, vk::Image image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);

    private:
        vk::Image m_image;
        VmaAllocation m_allocation;
        vk::ImageView m_view;

        vk::Sampler m_sampler;

        uint32_t m_width;
        uint32_t m_height;
    };
}
