//
// Created by stumi on 11/06/21.
//

#ifndef PERSONAL_RENDERER_VULKANBUFFER_H
#define PERSONAL_RENDERER_VULKANBUFFER_H

#include "GFX/Resources/ResourceDescriptions.h"

#include "vk_mem_alloc.h"

#include <vulkan/vulkan.hpp>

namespace gfx
{
    auto BufferTypeToVulkan(BufferType type) -> vk::BufferUsageFlags;

    class Buffer
    {
    public:
        Buffer(BufferDesc desc);

        auto GetType() const -> BufferType;
        auto GetSize() const -> uint32_t;
        auto GetStride() const -> uint32_t;
        auto GetNumElements() const -> uint32_t;

        void SetAPIResource(vk::Buffer buffer, VmaAllocation allocation);
        auto GetAPIBuffer() const -> vk::Buffer;
        auto GetAPIAllocation() const -> VmaAllocation;

    private:
        BufferDesc m_desc;
        vk::Buffer m_buffer{};
        VmaAllocation m_allocation{};
    };
}  // namespace gfx

#endif  // PERSONAL_RENDERER_VULKANBUFFER_H
