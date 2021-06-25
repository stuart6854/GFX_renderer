//
// Created by stumi on 10/06/21.
//

#ifdef GFX_API_VULKAN

    #include "VulkanFramebuffer.h"

    #include "VulkanRenderSurface.h"

    #include "GFX/Debug.h"

namespace gfx
{
    Framebuffer::Framebuffer(RenderSurface* renderSurface)
    {
        m_desc.IsSwapChainTarget = true;
        m_desc.SwapChainTarget = renderSurface;

        GFX_ASSERT(renderSurface != nullptr, "Cannot create a framebuffer with a null RenderSurface!");

        Resize(renderSurface->GetWidth(), renderSurface->GetHeight(), true);
    }

    Framebuffer::Framebuffer(const FramebufferDesc& desc) : m_desc(desc)
    {
        GFX_ASSERT(m_desc.Width != 0 && m_desc.Height != 0, "Framebuffer width & height cannot be 0!");
        
        GFX_ASSERT(!m_desc.Attachments.empty(), "Cannot create framebuffer with 0 attachments!");

        // Create all image objects immediately
        for (auto& attachmentDesc : m_desc.Attachments)
        {
            if (Utils::IsDepthFormat(attachmentDesc.Format))
            {
            }
            else
            {
            }
        }

        Resize(m_desc.Width, m_desc.Height, true);
    }

    void Framebuffer::Resize(uint32_t width, uint32_t height, bool forceRecreate)
    {
        if (!forceRecreate && (m_desc.Width == width && m_desc.Height == height)) return;

        m_desc.Width = width;
        m_desc.Height = height;

        if (!m_desc.IsSwapChainTarget)
        {
            // Invalidate();
        }
        else
        {
            m_renderPass = m_desc.SwapChainTarget->GetRenderPass();
        }
    }

}  // namespace gfx

#endif