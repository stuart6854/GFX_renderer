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

    void VulkanResourceSetLayout::AddBinding(uint32_t binding, ResourceType type, uint32_t arraySize, ShaderStage shaderStage)
    {
        vk::DescriptorSetLayoutBinding resBinding{};
        resBinding.setBinding(binding);
        resBinding.setDescriptorCount(arraySize);
        resBinding.setDescriptorType(Utils::ToVulkanDescriptorType(type));
        resBinding.setStageFlags(VkUtils::ToVkShaderStage(shaderStage));

        m_bindings[binding] = resBinding;
    }

    void VulkanResourceSetLayout::Build()
    {
        auto* backend = VulkanBackend::Get();
        auto& device = backend->GetDevice();
        auto vkDevice = device.GetHandle();

        auto bindings = GetBindings();

        vk::DescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.setBindings(bindings);

        m_layout = vkDevice.createDescriptorSetLayout(layoutInfo);
    }

    auto VulkanResourceSetLayout::GetBindings() const -> std::vector<vk::DescriptorSetLayoutBinding>
    {
        std::vector<vk::DescriptorSetLayoutBinding> bindings;

        for (const auto& [b, binding] : m_bindings)
            bindings.push_back(binding);

        return bindings;
    }
}
