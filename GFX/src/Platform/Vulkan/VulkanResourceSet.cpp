﻿#include "VulkanResourceSet.h"

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

        auto bindings = vkSetLayout->GetBindings();
        for (auto& binding : bindings)
        {
            auto& write = m_descriptorWrites[binding.binding];
            write.setDstSet(m_descriptorSet);
            write.setDstBinding(binding.binding);
            write.setDescriptorCount(binding.descriptorCount);
            write.setDescriptorType(binding.descriptorType);
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

        auto bindings = vkSetLayout->GetBindings();
        for (auto& binding : bindings)
        {
            auto& write = m_descriptorWrites[binding.binding];
            write.setDstSet(m_descriptorSet);
            write.setDstBinding(binding.binding);
            write.setDescriptorCount(binding.descriptorCount);
            write.setDescriptorType(binding.descriptorType);
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

        m_bufferInfos = vkOther.m_bufferInfos;
        m_imageInfos = vkOther.m_imageInfos;
        m_descriptorWrites = vkOther.m_descriptorWrites;
        for (auto& [binding, write] : m_descriptorWrites)
        {
            write.setDstSet(m_descriptorSet);

            if (write.descriptorType == vk::DescriptorType::eUniformBuffer)
                write.setBufferInfo(m_bufferInfos[write.dstBinding]);
            else if (write.descriptorType == vk::DescriptorType::eCombinedImageSampler)
                write.setImageInfo(m_imageInfos[write.dstBinding]);
        }
    }

    void VulkanResourceSet::SetUniformBuffer(uint32_t binding, UniformBuffer* buffer)
    {
        // Check if this set has this binding
        if (m_descriptorWrites.find(binding) == m_descriptorWrites.end())
            return;

        auto& write = m_descriptorWrites[binding];

        auto* vkBuffer = static_cast<VulkanBuffer*>(buffer->GetBuffer());

        auto& bufferInfo = m_bufferInfos[binding];
        bufferInfo.setBuffer(vkBuffer->GetHandle());
        bufferInfo.setOffset(0);
        bufferInfo.setRange(vkBuffer->GetSize());

        write.setBufferInfo(bufferInfo);
    }

    void VulkanResourceSet::SetTextureSampler(uint32_t binding, Texture* texture)
    {
        auto& write = m_descriptorWrites[binding];

        auto* vkTexture = static_cast<VulkanTexture*>(texture);

        auto& imageInfo = m_imageInfos[binding];
        imageInfo.setImageView(vkTexture->GetView());
        imageInfo.setSampler(vkTexture->GetSampler());
        if (texture->GetFormat() == TextureFormat::eDepth24Stencil8 || texture->GetFormat() == TextureFormat::eDepth32f)
            imageInfo.setImageLayout(vk::ImageLayout::eDepthStencilReadOnlyOptimal);
        else
            imageInfo.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

        write.setImageInfo(imageInfo);
    }

    void VulkanResourceSet::UpdateBindings()
    {
        auto& device = VulkanBackend::Get()->GetDevice();
        auto vkDevice = device.GetHandle();

        std::vector<vk::WriteDescriptorSet> writes;
        for (const auto& [binding, write] : m_descriptorWrites)
            writes.push_back(write);

        vkDevice.updateDescriptorSets(writes, {});
    }
}
