#pragma once

#include "GFX/Resources/ResourceSetLayout.h"

#include <vulkan/vulkan.hpp>

#include <cstdint>
#include <vector>

namespace gfx
{
    class VulkanResourceSetLayout : public ResourceSetLayout
    {
    public:
        VulkanResourceSetLayout();
        ~VulkanResourceSetLayout();

        auto GetHandle() const -> vk::DescriptorSetLayout { return m_layout; }

        void AddBinding(uint32_t binding, ResourceType type, ShaderStage shaderStage) override;

        void Build() override;

        auto GetBindings() const -> const std::vector<vk::DescriptorSetLayoutBinding>& { return m_bindings; }

    private:
        vk::DescriptorSetLayout m_layout;

        std::vector<vk::DescriptorSetLayoutBinding> m_bindings;
    };
}
