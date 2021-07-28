#pragma once

#include "GFX/Resources/CommandBuffer.h"

#include <vulkan/vulkan.hpp>

#include <cstdint>
#include <vector>

namespace gfx
{
    class SwapChain;
    class Pipeline;
    class VulkanPipeline;

    class VulkanCommandBuffer : public CommandBuffer
    {
    public:
        VulkanCommandBuffer(uint32_t count = 0);
        ~VulkanCommandBuffer() override;

        auto GetHandle() const -> vk::CommandBuffer { return m_currentCmdBuffer; }
        auto GetFence() const -> vk::Fence { return m_currentFence; }

        void Begin() override;
        void End() override;

        void SetViewport(const Viewport& viewport) override;
        void SetScissor(const Scissor& scissor) override;
        void SetLineWidth(float width) override;

        void BeginRenderPass(Framebuffer* framebuffer) override;
        void EndRenderPass() override;

        void BindPipeline(Pipeline* pipeline) override;

        void BindVertexBuffer(Buffer* buffer) override;
        void BindIndexBuffer(Buffer* buffer) override;

        void SetConstants(ShaderStage shaderStage, uint32_t offset, uint32_t size, const void* data) override;
        void BindResourceSets(uint32_t firstSet, const std::vector<ResourceSet*> sets) override;

        void Draw(uint32_t vertexCount) override;
        void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance) override;

    private:
        vk::CommandPool m_cmdPool;
        std::vector<vk::CommandBuffer> m_cmdBuffers;
        std::vector<vk::Fence> m_fences;
        uint32_t m_index = 0;

        vk::CommandBuffer m_currentCmdBuffer;
        vk::Fence m_currentFence;

        /* State */
        VulkanPipeline* m_boundPipeline;
    };
}
