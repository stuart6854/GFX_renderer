#include "VulkanResourceSet.h"

#include "GFX/Resources/UniformBuffer.h"

#include "VulkanBackend.h"
#include "VulkanResourceSetLayout.h"
#include "VulkanBuffer.h"

namespace gfx
{
    VulkanResourceSet::VulkanResourceSet(uint32_t set, ResourceSetLayout* setLayout)
        : m_set(set)
    {
        auto& device = VulkanBackend::Get()->GetDevice();

        auto* vkSetLayout = static_cast<VulkanResourceSetLayout*>(setLayout);
        m_descriptorSet = device.AllocateDescriptorSet(vkSetLayout->GetHandle());

        auto& bindings = vkSetLayout->GetBindings();
        m_descriptorWrites.resize(bindings.size());
        for (int i = 0; i < m_descriptorWrites.size(); i++)
        {
            auto& binding = bindings[i];

            auto& write = m_descriptorWrites[i];
            write.setDstSet(m_descriptorSet);
            write.setDstBinding(binding.binding);
            write.setDescriptorCount(binding.descriptorCount);
            write.setDescriptorType(binding.descriptorType);
        }
    }

    VulkanResourceSet::~VulkanResourceSet()
    {
    }

    void VulkanResourceSet::SetUniformBuffer(uint32_t binding, UniformBuffer* buffer)
    {
        auto& write = m_descriptorWrites[binding];

        auto* vkBuffer = static_cast<VulkanBuffer*>(buffer->GetBuffer());

        auto& bufferInfo = m_bufferInfos[binding];
        bufferInfo.setBuffer(vkBuffer->GetHandle());
        // bufferInfo.setOffset(0);
        bufferInfo.setRange(vkBuffer->GetSize());

        write.setBufferInfo(bufferInfo);
    }

    void VulkanResourceSet::UpdateBindings()
    {
        auto& device = VulkanBackend::Get()->GetDevice();
        auto vkDevice = device.GetHandle();

        vkDevice.updateDescriptorSets(m_descriptorWrites, {});
    }
}
