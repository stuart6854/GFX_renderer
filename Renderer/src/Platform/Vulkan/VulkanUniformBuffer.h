//
// Created by stumi on 20/06/21.
//

#ifndef PERSONAL_RENDERER_VULKANUNIFORMBUFFER_H
#define PERSONAL_RENDERER_VULKANUNIFORMBUFFER_H

#include "VulkanBuffer.h"

#include <vulkan/vulkan.hpp>

namespace gfx
{
    class UniformBuffer
    {
    public:
        UniformBuffer(uint32_t size, uint32_t binding);

        void SetData(const void* data, uint32_t size, uint32_t offset = 0);

        auto GetBinding() const -> uint32_t { return m_binding; }

        auto GetDescriptorBufferInfo() const -> const vk::DescriptorBufferInfo& { return m_descriptorInfo; }

    private:
    private:
        std::shared_ptr<Buffer> m_buffer;
        vk::DescriptorBufferInfo m_descriptorInfo = {};
        uint32_t m_size = 0;
        uint32_t m_binding = 0;
        vk::ShaderStageFlagBits m_shaderStage = {};
    };
}  // namespace gfx

#endif  // PERSONAL_RENDERER_VULKANUNIFORMBUFFER_H
