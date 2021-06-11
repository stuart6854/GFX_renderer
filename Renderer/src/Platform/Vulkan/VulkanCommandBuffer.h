//
// Created by stumi on 09/06/21.
//

#ifndef PERSONAL_RENDERER_VULKANCOMMANDBUFFER_H
#define PERSONAL_RENDERER_VULKANCOMMANDBUFFER_H

#include "GFX/Resources/Buffer.h"

#include <vulkan/vulkan.hpp>

namespace gfx
{
    class CommandBuffer
    {
    public:
        CommandBuffer();
        ~CommandBuffer();

        void Begin();
        void End();

        void BeginRenderPass(vk::RenderPassBeginInfo& beginInfo);
        void EndRenderPass();

        void BindVertexBuffer(Buffer& buffer);
        void BindIndexBuffer(Buffer& buffer);

        auto GetAPIResource() -> vk::CommandBuffer;
        void SetAPIResource(vk::CommandBuffer cmdBuffer);
        auto GetFence() -> vk::Fence;

    private:
        vk::CommandPool m_cmdPool;
        vk::CommandBuffer m_cmdBuffer;
        vk::Fence m_fence;
    };

}  // namespace gfx

#endif  // PERSONAL_RENDERER_VULKANCOMMANDBUFFER_H
