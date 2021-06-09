//
// Created by stumi on 09/06/21.
//

#include "VulkanCommandBuffer.h"

namespace gfx
{
    auto CommandBuffer::GetAPIResource() -> vk::CommandBuffer { return m_cmdBuffer; }

    void CommandBuffer::SetAPIResource(vk::CommandBuffer cmdBuffer) { m_cmdBuffer = cmdBuffer; }

}  // namespace gfx