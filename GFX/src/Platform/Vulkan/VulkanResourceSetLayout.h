#pragma once

#include "GFX/Resources/ResourceSetLayout.h"

#include <vulkan/vulkan.hpp>

#include <cstdint>
#include <unordered_map>

namespace gfx
{
    class VulkanResourceSetLayout : public ResourceSetLayout
    {
    public:
        VulkanResourceSetLayout() = default;
        ~VulkanResourceSetLayout() override = default;

        auto GetHandle() const -> vk::DescriptorSetLayout { return m_layout; }

        void AddBinding(uint32_t binding, ResourceType type, size_t arraySize, ShaderStage shaderStage) override;

        void Build() override;

        auto GetBindings() const -> std::vector<vk::DescriptorSetLayoutBinding>;

    private:
        vk::DescriptorSetLayout m_layout;

        std::unordered_map<uint32_t, vk::DescriptorSetLayoutBinding> m_bindings;
    };
}
