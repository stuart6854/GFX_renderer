#pragma once

#include "GFX/Resources/ResourceSet.h"

#include <vulkan/vulkan.hpp>

#include <cstdint>
#include <vector>

namespace gfx
{
    class VulkanResourceSet : public ResourceSet
    {
    public:
        VulkanResourceSet(uint32_t set, ResourceSetLayout* setLayout);
        ~VulkanResourceSet();

        auto GetHandle() const -> vk::DescriptorSet { return m_descriptorSet; }

        void SetUniformBuffer(uint32_t binding, UniformBuffer* buffer) override;

        void UpdateBindings() override;

    private:
        uint32_t m_set;
        vk::DescriptorSet m_descriptorSet;

        std::unordered_map<uint32_t, vk::DescriptorBufferInfo> m_bufferInfos;
        std::vector<vk::WriteDescriptorSet> m_descriptorWrites;
    };
}
