//
// Created by stumi on 09/06/21.
//

#ifndef PERSONAL_RENDERER_VULKANCOMMANDBUFFER_H
#define PERSONAL_RENDERER_VULKANCOMMANDBUFFER_H

#include "GFX/Resources/Buffer.h"
#include "GFX/Resources/Pipeline.h"

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

        void BindPipeline(Pipeline& pipeline);
        void BindVertexBuffer(Buffer& buffer);
        void BindIndexBuffer(Buffer& buffer);

        void Draw(uint32_t vertexCount);
        void DrawIndexed(uint32_t indexCount);

        void CopyBuffer(vk::Buffer src, vk::Buffer dst, uint32_t size);

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
