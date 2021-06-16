//
// Created by stumi on 07/06/21.
//

#ifndef PERSONAL_RENDERER_VULKANRENDERCONTEXT_H
#define PERSONAL_RENDERER_VULKANRENDERCONTEXT_H

#include "VulkanCommandBuffer.h"
#include "VulkanDeviceContext.h"

#include "GFX/Resources/Buffer.h"
#include "GFX/Resources/Pipeline.h"
#include "GFX/Utility/Color.h"

#include <vulkan/vulkan.hpp>

namespace gfx
{
    /**
     * A Rendering Context handles rendering resources such as texture object, buffer objects, etc.
     * It also manages rendering state.
     *
     * A Rendering Context is attached to a device context. You can, for example, have two windows (two Device Contexts)
     * and one Rendering Context, and then you can direct the Rendering Context to each window by attaching the Rendering
     * Context to each window before drawing whatever you want.
     */
    class RenderContext
    {
    public:
        void Begin();
        void End();

        void BeginRenderPass(const Color& clearColor, Framebuffer* framebuffer);
        void EndRenderPass();

        void BindPipeline(Pipeline* pipeline);
        void BindVertexBuffer(Buffer* buffer);
        void BindIndexBuffer(Buffer* buffer);

        void Draw(uint32_t vertexCount);
        void DrawIndexed(uint32_t indexCount);

        void NextCommandBuffer();

        auto GetCommandBuffer() -> CommandBuffer&;

    private:
        std::array<CommandBuffer, FRAME_OVERLAP> m_cmdBuffers{};
        uint32_t m_activeCmdBufferIndex{};

        Pipeline* m_boundPipeline;
    };

}  // namespace gfx

#endif  // PERSONAL_RENDERER_VULKANRENDERCONTEXT_H
