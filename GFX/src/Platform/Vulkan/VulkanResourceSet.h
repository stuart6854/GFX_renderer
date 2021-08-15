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
        void SetTextureSampler(uint32_t binding, uint32_t index, Texture* texture) override;

        void UpdateBindings() override;

    private:
        struct ResourceDecl
        {
            uint32_t Binding = 0;
            vk::DescriptorType Type{};
            uint32_t Count = 1;
            gfx::UniformBuffer* Buffer = nullptr;
            std::vector<gfx::Texture*> Textures;

            vk::DescriptorBufferInfo BufferInfo{};
            std::vector<vk::DescriptorImageInfo> ImageInfos{};
        };

        auto GetLayoutBinding(uint32_t binding) -> vk::DescriptorSetLayoutBinding*;

    private:
        std::set<uint32_t> m_validBindings;
        std::vector<vk::DescriptorSetLayoutBinding> m_bindings;

        uint32_t m_set = 0;

        vk::DescriptorPool m_descriptorPool;
        vk::DescriptorSet m_descriptorSet;

        std::unordered_map<uint32_t, ResourceDecl> m_resources;
    };
}
