#pragma once

#include "GFX/Core/Base.h"
#include "GFX/Resources/Viewport.h"
#include "GFX/Resources/Scissor.h"
#include "Shader.h"

#include <cstdint>

namespace gfx
{
    class SwapChain;
    class Framebuffer;
    class Pipeline;
    class Buffer;
    class ResourceSet;

    class CommandBuffer
    {
    public:
        static auto Create(uint32_t count = 0) -> OwnedPtr<CommandBuffer>;

        virtual ~CommandBuffer() = default;

        virtual void Begin() = 0;
        virtual void End() = 0;

        virtual void SetViewport(const Viewport& viewport) = 0;
        virtual void SetScissor(const Scissor& scissor) = 0;

        virtual void BeginRenderPass(Framebuffer* framebuffer) = 0;
        virtual void EndRenderPass() = 0;

        virtual void BindPipeline(Pipeline* pipeline) = 0;

        virtual void BindVertexBuffer(Buffer* buffer) = 0;
        virtual void BindIndexBuffer(Buffer* buffer) = 0;

        virtual void SetConstants(ShaderStage shaderStage, uint32_t offset, uint32_t size, const void* data) = 0;
        virtual void BindResourceSets(uint32_t firstSet, const std::vector<ResourceSet*> sets) = 0;

        virtual void Draw(uint32_t vertexCount) = 0;
        virtual void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance) = 0;
    };
}
