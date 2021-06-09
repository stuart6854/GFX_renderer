//
// Created by stumi on 09/06/21.
//

#ifndef PERSONAL_RENDERER_VULKANCOMMANDBUFFER_H
#define PERSONAL_RENDERER_VULKANCOMMANDBUFFER_H

#include <vulkan/vulkan.hpp>

namespace gfx
{
    class CommandBuffer
    {
    public:
        auto GetAPIResource() -> vk::CommandBuffer;
        void SetAPIResource(vk::CommandBuffer cmdBuffer);

    private:
        vk::CommandBuffer m_cmdBuffer;
    };

}  // namespace gfx

#endif  // PERSONAL_RENDERER_VULKANCOMMANDBUFFER_H
