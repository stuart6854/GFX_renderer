//
// Created by stumi on 07/06/21.
//

#ifdef GFX_API_VULKAN

    #include "VulkanRenderContext.h"

namespace gfx
{
    void RenderContext::SetVertexBuffer() {}

    void RenderContext::SetIndexBuffer() {}

    void RenderContext::Draw() {}

    void RenderContext::DrawIndexed() {}

    auto RenderContext::GetCommandBuffer() -> CommandBuffer { return m_cmdBuffer; }

}  // namespace gfx

#endif