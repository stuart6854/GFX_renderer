#pragma once

#include "GFX/Resources/ResourceSet.h"

#include <vulkan/vulkan.hpp>

#include <cstdint>
#include <set>

namespace gfx
{
    class VulkanResourceSet : public ResourceSet
    {
    public:
        VulkanResourceSet(uint32_t frameIndex, uint32_t set, ResourceSetLayout* setLayout);
        VulkanResourceSet(uint32_t set, ResourceSetLayout* setLayout);
        ~VulkanResourceSet();

        auto GetHandle() const -> vk::DescriptorSet { return m_descriptorSet; }

        void CopyBindings(const ResourceSet& other) override;

        void SetUniformBuffer(uint32_t binding, UniformBuffer* buffer) override;
        void SetTextureSampler(uint32_t binding, Texture* texture) override;

        void UpdateBindings() override;

    private:
        struct ResourceDecl
        {
            uint32_t Binding = 0;
            vk::DescriptorType Type{};
            gfx::UniformBuffer* Buffer = nullptr;
            gfx::Texture* Texture = nullptr;

            vk::DescriptorBufferInfo BufferInfo{};
            vk::DescriptorImageInfo ImageInfo{};
        };

    private:
        std::set<uint32_t> m_validBindings;

        uint32_t m_set = 0;

        vk::DescriptorPool m_descriptorPool;
        vk::DescriptorSet m_descriptorSet;

        std::unordered_map<uint32_t, ResourceDecl> m_resources;
    };
}
