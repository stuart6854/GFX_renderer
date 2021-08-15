#include "VulkanResourceSet.h"

#include "GFX/Resources/UniformBuffer.h"

#include "VulkanBackend.h"
#include "VulkanResourceSetLayout.h"
#include "VulkanBuffer.h"
#include "VulkanTexture.h"

namespace gfx
{
    VulkanResourceSet::VulkanResourceSet(const uint32_t frameIndex, const uint32_t set, ResourceSetLayout* setLayout)
        : m_set(set)
    {
        auto& device = VulkanBackend::Get()->GetDevice();

        auto* vkSetLayout = static_cast<VulkanResourceSetLayout*>(setLayout);
        m_descriptorSet = device.AllocateDescriptorSet(frameIndex, vkSetLayout->GetHandle());

        m_bindings = vkSetLayout->GetBindings();
        for (auto& binding : m_bindings)
        {
            //     auto& write = m_descriptorWrites[binding.binding];
            //     write.setDstSet(m_descriptorSet);
            //     write.setDstBinding(binding.binding);
            //     write.setDescriptorCount(binding.descriptorCount);
            //     write.setDescriptorType(binding.descriptorType);

            m_validBindings.emplace(binding.binding);
        }
    }

    VulkanResourceSet::VulkanResourceSet(uint32_t set, ResourceSetLayout* setLayout)
        : m_set(set)
    {
        auto& device = VulkanBackend::Get()->GetDevice();
        const auto vkDevice = device.GetHandle();

        std::vector<vk::DescriptorPoolSize> poolSizes = {
            { vk::DescriptorType::eUniformBuffer, 25 },
            { vk::DescriptorType::eCombinedImageSampler, 25 },
        };

        vk::DescriptorPoolCreateInfo poolInfo{};
        poolInfo.setPoolSizes(poolSizes);
        poolInfo.setMaxSets(1);
        m_descriptorPool = vkDevice.createDescriptorPool(poolInfo);

        auto* vkSetLayout = static_cast<VulkanResourceSetLayout*>(setLayout);
        const auto setHandle = vkSetLayout->GetHandle();

        vk::DescriptorSetAllocateInfo allocInfo{};
        allocInfo.setDescriptorPool(m_descriptorPool);
        allocInfo.setDescriptorSetCount(1);
        allocInfo.setSetLayouts(setHandle);

        m_descriptorSet = vkDevice.allocateDescriptorSets(allocInfo)[0];

        m_bindings = vkSetLayout->GetBindings();
        for (auto& binding : m_bindings)
        {
            //     auto& write = m_descriptorWrites[binding.binding];
            //     write.setDstSet(m_descriptorSet);
            //     write.setDstBinding(binding.binding);
            //     write.setDescriptorCount(binding.descriptorCount);
            //     write.setDescriptorType(binding.descriptorType);
            m_validBindings.emplace(binding.binding);
        }
    }

    VulkanResourceSet::~VulkanResourceSet()
    {
        if (m_descriptorPool)
        {
            auto* backend = VulkanBackend::Get();
            const auto vkDevice = backend->GetDevice().GetHandle();

            vkDevice.destroy(m_descriptorPool);
        }
    }

    void VulkanResourceSet::CopyBindings(const ResourceSet& other)
    {
        const auto& vkOther = static_cast<const VulkanResourceSet&>(other);

        m_validBindings = vkOther.m_validBindings;
        m_resources = vkOther.m_resources;
    }

    void VulkanResourceSet::SetUniformBuffer(uint32_t binding, UniformBuffer* buffer)
    {
        // Check if this binding is valid for this set
        if (m_validBindings.find(binding) == m_validBindings.end())
            return;

        auto* vkBuffer = static_cast<VulkanBuffer*>(buffer->GetBuffer());

        auto& resource = m_resources[binding];
        resource.Binding = binding;
        resource.Type = vk::DescriptorType::eUniformBuffer;
        resource.Buffer = buffer;
        resource.BufferInfo = vkBuffer->GetBufferInfo();
    }

    void VulkanResourceSet::SetTextureSampler(uint32_t binding, uint32_t index, Texture* texture)
    {
        // Check if this binding is valid for this set
        if (m_validBindings.find(binding) == m_validBindings.end())
            return;

        auto* vkTexture = static_cast<VulkanTexture*>(texture);

        auto& resource = m_resources[binding];
        resource.Binding = binding;
        resource.Type = vk::DescriptorType::eCombinedImageSampler;
        resource.Count = index + 1;

        if (resource.Textures.size() < index + 1)
            resource.Textures.resize(index + 1);
        if (resource.ImageInfos.size() < index + 1)
            resource.ImageInfos.resize(index + 1);

        resource.Textures[index] = texture;
        resource.ImageInfos[index] = vkTexture->GetImageInfo();
    }

    void VulkanResourceSet::UpdateBindings()
    {
        auto& device = VulkanBackend::Get()->GetDevice();
        auto vkDevice = device.GetHandle();

        // for (const auto& [binding, write] : m_descriptorWrites)
        // {
        //     if (!write.dstSet)
        //         continue;
        //     if (write.pImageInfo == nullptr && write.pBufferInfo == nullptr)
        //         continue;
        //
        //     writes.push_back(write);
        // }

        std::vector<vk::WriteDescriptorSet> writes;
        for (const auto& [binding, resource] : m_resources)
        {
            auto& write = writes.emplace_back();
            write.setDstSet(m_descriptorSet);
            write.setDstBinding(binding);
            write.setDescriptorType(resource.Type);
            write.setDescriptorCount(resource.Count);

            if (resource.Type == vk::DescriptorType::eUniformBuffer)
            {
                write.setBufferInfo(resource.BufferInfo);
            }
            else if (resource.Type == vk::DescriptorType::eCombinedImageSampler)
            {
                write.setImageInfo(resource.ImageInfos);
            }
        }

        vkDevice.updateDescriptorSets(writes, {});
    }

    auto VulkanResourceSet::GetLayoutBinding(uint32_t binding) -> vk::DescriptorSetLayoutBinding*
    {
        for (auto& layoutBinding : m_bindings)
        {
            if (layoutBinding.binding == binding)
                return &layoutBinding;
        }

        return nullptr;
    }
}
