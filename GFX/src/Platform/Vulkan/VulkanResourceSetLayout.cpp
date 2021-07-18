#include "VulkanResourceSetLayout.h"

#include "VulkanBackend.h"
#include "VulkanUtils.h"

namespace gfx
{
    namespace Utils
    {
        auto ToVulkanDescriptorType(ResourceType type) -> vk::DescriptorType
        {
            switch (type)
            {
                default:
                case ResourceType::eNone: break;
                case ResourceType::eUniformBuffer: return vk::DescriptorType::eUniformBuffer;
                case ResourceType::eTextureSampler: return vk::DescriptorType::eCombinedImageSampler;
            }
            return {};
        }
    }

    VulkanResourceSetLayout::VulkanResourceSetLayout()
    {
    }

    VulkanResourceSetLayout::~VulkanResourceSetLayout()
    {
        auto* backend = VulkanBackend::Get();
        auto& device = backend->GetDevice();
        auto vkDevice = device.GetHandle();

        vkDevice.destroy(m_layout);
    }

    void VulkanResourceSetLayout::AddBinding(uint32_t binding, ResourceType type, ShaderStage shaderStage)
    {
        // Resize vector for binding
        if (m_bindings.size() <= binding)
            m_bindings.resize(binding + 1);

        vk::DescriptorSetLayoutBinding resBinding{};
        resBinding.setBinding(binding);
        resBinding.setDescriptorCount(1);
        resBinding.setDescriptorType(Utils::ToVulkanDescriptorType(type));
        resBinding.setStageFlags(VkUtils::ToVkShaderStage(shaderStage));

        m_bindings[binding] = resBinding;
    }

    void VulkanResourceSetLayout::Build()
    {
        auto* backend = VulkanBackend::Get();
        auto& device = backend->GetDevice();
        auto vkDevice = device.GetHandle();

        vk::DescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.setBindings(m_bindings);

        m_layout = vkDevice.createDescriptorSetLayout(layoutInfo);
    }
}
