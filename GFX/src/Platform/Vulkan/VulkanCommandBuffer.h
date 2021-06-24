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

        void BindPipeline(Pipeline* pipeline);
        void BindVertexBuffer(Buffer* buffer);
        void BindIndexBuffer(Buffer* buffer);

        void PushConstants(vk::PipelineLayout layout, ShaderStage stage, uint32_t offset, uint32_t size, const void* data);
        void BindDescriptorSets(vk::PipelineBindPoint bindPoint,
                                vk::PipelineLayout layout,
                                uint32_t firstSet,
                                const std::vector<vk::DescriptorSet>& sets,
                                const std::vector<uint32_t>& dynamicOffsets);

        void Draw(uint32_t vertexCount);
        void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance);

        void CopyBuffer(vk::Buffer src, vk::Buffer dst, uint32_t size);
        void CopyBufferToImage(vk::Buffer src, vk::Image dst, vk::ImageLayout layout, vk::BufferImageCopy region);

        void ImageMemoryBarrier(vk::Image image,
                                vk::AccessFlags srcAccessMask,
                                vk::AccessFlags dstAccessMask,
                                vk::ImageLayout oldLayout,
                                vk::ImageLayout newLayout,
                                vk::PipelineStageFlags srcStageMask,
                                vk::PipelineStageFlags dstStageMask,
                                vk::ImageSubresourceRange range);

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
