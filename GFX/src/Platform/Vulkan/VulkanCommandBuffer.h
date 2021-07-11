#pragma once

#include "GFX/Resources/CommandBuffer.h"

#include <vulkan/vulkan.hpp>

#include <cstdint>
#include <vector>

namespace gfx
{
    class SwapChain;

    class VulkanCommandBuffer : public CommandBuffer
    {
    public:
        VulkanCommandBuffer(uint32_t count = 0);
        ~VulkanCommandBuffer();

        auto GetHandle() const -> vk::CommandBuffer { return m_currentCmdBuffer; }
        auto GetFence() const -> vk::Fence { return m_currentFence; }

        void Begin() override;
        void End() override;

        void BeginRenderPass(Framebuffer* framebuffer) override;
        void EndRenderPass() override;

    private:
        vk::CommandPool m_cmdPool;
        std::vector<vk::CommandBuffer> m_cmdBuffers;
        std::vector<vk::Fence> m_fences;
        uint32_t m_index = 0;

        vk::CommandBuffer m_currentCmdBuffer;
        vk::Fence m_currentFence;
    };
}
