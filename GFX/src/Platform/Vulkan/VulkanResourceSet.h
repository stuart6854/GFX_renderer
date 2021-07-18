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
        VulkanResourceSet(ResourceSetLayout* setLayout);
        ~VulkanResourceSet();

        auto GetHandle() const -> vk::DescriptorSet { return m_descriptorSet; }

    private:
        vk::DescriptorSet m_descriptorSet;
    };
}
