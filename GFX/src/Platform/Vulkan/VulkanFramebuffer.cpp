//
// Created by stumi on 10/06/21.
//

#ifdef GFX_API_VULKAN

    #include "VulkanFramebuffer.h"

    #include "VulkanCore.h"
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
            ImageDesc imageDesc{};
            imageDesc.Format = attachmentDesc.Format;
            imageDesc.Usage = ImageUsage::eAttachment;
            imageDesc.Width = m_desc.Width;
            imageDesc.Height = m_desc.Height;

            if (Utils::IsDepthFormat(attachmentDesc.Format))
            {
                m_depthAttachmentImage = std::make_shared<Image>(imageDesc);
            }
            else
            {
                m_attachmentImages.emplace_back(std::make_shared<Image>(imageDesc));
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
            Invalidate();
        }
        else
        {
            m_renderPass = m_desc.SwapChainTarget->GetRenderPass();

            m_clearValues.clear();

            const std::array clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
            m_clearValues.emplace_back().setColor(clearColor);
            m_clearValues.emplace_back().setDepthStencil({ 1.0f, 0 });
        }
    }

    auto Framebuffer::GetFramebuffer() const -> vk::Framebuffer
    {
        if (!m_desc.IsSwapChainTarget) return m_framebuffer;

        return m_desc.SwapChainTarget->GetCurrentFramebuffer();
    }

    void Framebuffer::Invalidate()
    {
        const auto& device = Vulkan::GetDevice();

        if (m_framebuffer)
        {
            // Destroy pre-existing framebuffer & images
            device.destroy(m_framebuffer);

            for (auto& image : m_attachmentImages)
            {
                image->Release();
            }

            if (m_depthAttachmentImage)
            {
                m_depthAttachmentImage->Release();
            }
        }

        /* RenderPass */

        std::vector<vk::AttachmentDescription> attachmentDescriptions;
        std::vector<vk::AttachmentReference> colorAttachmentsReferences;
        vk::AttachmentReference depthAttachmentReference;

        m_clearValues.resize(m_desc.Attachments.size());

        const auto& clearColor = m_desc.ClearColor;
        const std::array clearColorArray = { clearColor.r, clearColor.g, clearColor.b, 1.0f };

        uint32_t attachmentIndex = 0;
        for (const auto& attachmentDesc : m_desc.Attachments)
        {
            if (Utils::IsDepthFormat(attachmentDesc.Format))
            {
                auto& desc = m_depthAttachmentImage->GetDesc();
                desc.Width = m_desc.Width;
                desc.Height = m_desc.Height;
                m_depthAttachmentImage->Invalidate();

                auto& attachmentReference = attachmentDescriptions.emplace_back();
                attachmentReference.setFormat(Utils::VulkanTextureFormat(attachmentDesc.Format));
                attachmentReference.setSamples(vk::SampleCountFlagBits::e1);
                attachmentReference.setLoadOp(m_desc.ClearOnLoad ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eLoad);
                attachmentReference.setStoreOp(vk::AttachmentStoreOp::eStore);
                attachmentReference.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
                attachmentReference.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
                attachmentReference.setInitialLayout(m_desc.ClearOnLoad ? vk::ImageLayout::eUndefined : vk::ImageLayout::eDepthStencilReadOnlyOptimal);
                if (attachmentDesc.Format == ImageFormat::eDepth24Stencil8 || true)
                {
                    attachmentReference.setFinalLayout(vk::ImageLayout::eDepthStencilReadOnlyOptimal);
                    depthAttachmentReference = { attachmentIndex, vk::ImageLayout::eDepthStencilAttachmentOptimal };
                }

                m_clearValues[attachmentIndex].setDepthStencil({ 1.0f, 0 });
            }
            else
            {
                auto colorAttachment = m_attachmentImages[attachmentIndex];
                auto& desc = colorAttachment->GetDesc();
                desc.Width = m_desc.Width;
                desc.Height = m_desc.Height;

                colorAttachment->Invalidate();

                auto& attachmentReference = attachmentDescriptions.emplace_back();
                attachmentReference.setFormat(Utils::VulkanTextureFormat(attachmentDesc.Format));
                attachmentReference.setSamples(vk::SampleCountFlagBits::e1);
                attachmentReference.setLoadOp(m_desc.ClearOnLoad ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eLoad);
                attachmentReference.setStoreOp(vk::AttachmentStoreOp::eStore);
                attachmentReference.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
                attachmentReference.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
                attachmentReference.setInitialLayout(m_desc.ClearOnLoad ? vk::ImageLayout::eUndefined : vk::ImageLayout::eShaderReadOnlyOptimal);
                attachmentReference.setFinalLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

                colorAttachmentsReferences.emplace_back(vk::AttachmentReference{ attachmentIndex, vk::ImageLayout::eColorAttachmentOptimal });

                m_clearValues[attachmentIndex].setColor(clearColorArray);
            }

            attachmentIndex++;
        }

        vk::SubpassDescription subPassDescription{};
        subPassDescription.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);
        subPassDescription.setColorAttachments(colorAttachmentsReferences);
        if (m_depthAttachmentImage) subPassDescription.setPDepthStencilAttachment(&depthAttachmentReference);

        // TODO: Dependencies?
        vk::SubpassDependency subPassDependency{};
        subPassDependency.setSrcSubpass(VK_SUBPASS_EXTERNAL);
        subPassDependency.setDstSubpass(0);
        subPassDependency.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests);
        subPassDependency.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests);
        subPassDependency.setSrcAccessMask({});
        subPassDependency.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite);

        vk::RenderPassCreateInfo renderPassInfo{};
        renderPassInfo.setAttachments(attachmentDescriptions);
        renderPassInfo.setSubpasses(subPassDescription);
        renderPassInfo.setDependencies(subPassDependency);

        m_renderPass = device.createRenderPass(renderPassInfo);

        /* Framebuffer */
        std::vector<vk::ImageView> attachments(m_attachmentImages.size());
        for (uint32_t i = 0; i < m_attachmentImages.size(); i++)
        {
            const auto image = m_attachmentImages.at(i);
            attachments[i] = image->GetVulkanView();
            GFX_ASSERT(attachments[i]);
        }

        if (m_depthAttachmentImage)
        {
            attachments.emplace_back(m_depthAttachmentImage->GetVulkanView());
            GFX_ASSERT(attachments.back());
        }

        vk::FramebufferCreateInfo framebufferInfo{};
        framebufferInfo.setRenderPass(m_renderPass);
        framebufferInfo.setAttachments(attachments);
        framebufferInfo.setWidth(m_desc.Width);
        framebufferInfo.setHeight(m_desc.Height);
        framebufferInfo.setLayers(1);

        m_framebuffer = device.createFramebuffer(framebufferInfo);
    }

}  // namespace gfx

#endif