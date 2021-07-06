#pragma once

#include "GFX/Resources/Buffer.h"
#include "vk_mem_alloc.h"

#include <vulkan/vulkan.hpp>

#include <cstdint>

namespace gfx
{
    struct VulkanBuffer : public Buffer
    {
        uint32_t Size;
        vk::Buffer Buffer;
        VmaAllocation Allocation;
    };

    auto CreateBuffer() -> Buffer;
}