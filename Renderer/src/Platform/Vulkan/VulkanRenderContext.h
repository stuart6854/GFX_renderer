//
// Created by stumi on 07/06/21.
//

#ifndef PERSONAL_RENDERER_VULKANRENDERCONTEXT_H
#define PERSONAL_RENDERER_VULKANRENDERCONTEXT_H

#include "VulkanCommandBuffer.h"

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
        void SetVertexBuffer();
        void SetIndexBuffer();

        void Draw();
        void DrawIndexed();

        auto GetCommandBuffer() -> CommandBuffer;

    private:
        CommandBuffer m_cmdBuffer;
    };

}  // namespace gfx

#endif  // PERSONAL_RENDERER_VULKANRENDERCONTEXT_H
