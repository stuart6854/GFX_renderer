#include "VulkanResourceSet.h"

#include "VulkanBackend.h"
#include "VulkanResourceSetLayout.h"

namespace gfx
{
    VulkanResourceSet::VulkanResourceSet(ResourceSetLayout* setLayout)
    {
        auto& device = VulkanBackend::Get()->GetDevice();

        auto* vkSetLayout = static_cast<VulkanResourceSetLayout*>(setLayout);
        m_descriptorSet = device.AllocateDescriptorSet(vkSetLayout->GetHandle());
    }

    VulkanResourceSet::~VulkanResourceSet()
    {
    }
}
