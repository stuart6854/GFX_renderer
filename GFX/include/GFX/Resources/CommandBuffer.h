#pragma once

#include "GFX/Core/Base.h"

#include <cstdint>

namespace gfx
{
    class SwapChain;
    class Framebuffer;
    class Buffer;

    class CommandBuffer
    {
    public:
        static auto Create(uint32_t count = 0) -> OwnedPtr<CommandBuffer>;

        virtual ~CommandBuffer() = default;

        virtual void Begin() = 0;
        virtual void End() = 0;

        virtual void BeginRenderPass(Framebuffer* framebuffer) = 0;
        virtual void EndRenderPass() = 0;

        virtual void BindVertexBuffer(Buffer* buffer) = 0;
        virtual void BindIndexBuffer(Buffer* buffer) = 0;

        virtual void Draw(uint32_t vertexCount) = 0;
        virtual void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance) = 0;
    };
}
