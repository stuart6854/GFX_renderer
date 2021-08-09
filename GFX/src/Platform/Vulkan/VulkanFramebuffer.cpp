#include "VulkanFramebuffer.h"

#include "GFX/Debug.h"

#include "VulkanBackend.h"
#include "VulkanSwapChain.h"
#include "VulkanTexture.h"
#include "VulkanUtils.h"

namespace gfx
{
    VulkanFramebuffer::VulkanFramebuffer(SwapChain* swapChain)
    {
        m_swapChain = swapChain;
        m_isSwapChainTarget = true;

        Resize(swapChain->GetWidth(), swapChain->GetHeight(), true);
    }

    VulkanFramebuffer::VulkanFramebuffer(const FramebufferDesc& desc)
        : m_desc(desc)
    {
        GFX_ASSERT(desc.Width != 0 && desc.Height != 0, "Framebuffer width and height cannot be 0!");
        GFX_ASSERT(!desc.Attachments.empty(), "Cannot create framebuffer with 0 attachments!");

        m_isSwapChainTarget = false;

        // Create all image objects immediately
        for (auto& attachmentDesc : m_desc.Attachments)
        {
            if (!IsDepthFormat(attachmentDesc.Format))
                m_attachmentTextures.emplace_back(nullptr);
        }

        Resize(desc.Width, desc.Height, true);
    }

    VulkanFramebuffer::~VulkanFramebuffer()
    {
        auto* backend = VulkanBackend::Get();
        auto& device = backend->GetDevice();
        auto vkDevice = device.GetHandle();

        if (!m_isSwapChainTarget)
        {
            vkDevice.destroy(m_framebuffer);
            vkDevice.destroy(m_renderPass);
        }
    }

    auto VulkanFramebuffer::GetBeginInfo() const -> vk::RenderPassBeginInfo
    {
        auto* vkSwapChain = static_cast<VulkanSwapChain*>(m_swapChain);

        vk::RenderPassBeginInfo beginInfo{};
        beginInfo.setClearValues(m_clearValues);
        beginInfo.setFramebuffer(!m_isSwapChainTarget ? m_framebuffer : vkSwapChain->GetCurrentFramebuffer());
        beginInfo.setRenderPass(m_renderPass);
        beginInfo.renderArea.offset.setX(0);
        beginInfo.renderArea.offset.setY(0);
        beginInfo.renderArea.extent.setWidth(m_width);
        beginInfo.renderArea.extent.setHeight(m_height);
        return beginInfo;
    }

    void VulkanFramebuffer::Resize(uint32_t width, uint32_t height, bool forceRecreate)
    {
        if (!forceRecreate && (m_width == width && m_height == height)) return;

        m_desc.Width = m_width = width;
        m_desc.Height = m_height = height;

        if (!m_isSwapChainTarget)
        {
            Invalidate();
        }
        else
        {
            auto* vkSwapChain = static_cast<VulkanSwapChain*>(m_swapChain);

            m_renderPass = vkSwapChain->GetRenderPass();

            m_clearValues.clear();

            const std::array clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
            m_clearValues.emplace_back().setColor(clearColor);
            m_clearValues.emplace_back().setDepthStencil({ 1.0f, 0 });
        }
    }

    void VulkanFramebuffer::Invalidate()
    {
        auto* backend = VulkanBackend::Get();
        auto& device = backend->GetDevice();
        auto vkDevice = device.GetHandle();

        if (m_framebuffer)
        {
            // Destroy pre-existing framebuffer & images
            vkDevice.destroy(m_framebuffer);

            for (auto& image : m_attachmentTextures)
            {
                image.reset();
            }

            if (m_depthAttachentTexture)
            {
                m_depthAttachentTexture.reset();
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
            if (IsDepthFormat(attachmentDesc.Format))
            {
                TextureDesc textureDesc{};
                textureDesc.Format = attachmentDesc.Format;
                textureDesc.Usage = TextureUsage::eAttachment;
                textureDesc.Width = m_desc.Width;
                textureDesc.Height = m_desc.Height;
                m_depthAttachentTexture = Texture::Create(textureDesc);

                auto& attachmentReference = attachmentDescriptions.emplace_back();
                attachmentReference.setFormat(VkUtils::ToVkTextureFormat(textureDesc.Format));
                attachmentReference.setSamples(vk::SampleCountFlagBits::e1);
                attachmentReference.setLoadOp(m_desc.ClearOnLoad ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eLoad);
                attachmentReference.setStoreOp(vk::AttachmentStoreOp::eStore);
                attachmentReference.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
                attachmentReference.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
                attachmentReference.setInitialLayout(m_desc.ClearOnLoad ? vk::ImageLayout::eUndefined : vk::ImageLayout::eDepthStencilReadOnlyOptimal);
                if (textureDesc.Format == TextureFormat::eDepth24Stencil8 || true)
                {
                    attachmentReference.setFinalLayout(vk::ImageLayout::eDepthStencilReadOnlyOptimal);
                    depthAttachmentReference = { attachmentIndex, vk::ImageLayout::eDepthStencilAttachmentOptimal };
                }

                m_clearValues[attachmentIndex].setDepthStencil({ 1.0f, 0 });
            }
            else
            {
                TextureDesc textureDesc{};
                textureDesc.Format = attachmentDesc.Format;
                textureDesc.Usage = TextureUsage::eAttachment;
                textureDesc.Width = m_desc.Width;
                textureDesc.Height = m_desc.Height;
                m_attachmentTextures[attachmentIndex] = Texture::Create(textureDesc);

                auto& attachmentReference = attachmentDescriptions.emplace_back();
                attachmentReference.setFormat(VkUtils::ToVkTextureFormat(textureDesc.Format));
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
        if (m_depthAttachentTexture) subPassDescription.setPDepthStencilAttachment(&depthAttachmentReference);

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

        m_renderPass = vkDevice.createRenderPass(renderPassInfo);

        /* Framebuffer */
        std::vector<vk::ImageView> attachments(m_attachmentTextures.size());
        for (uint32_t i = 0; i < m_attachmentTextures.size(); i++)
        {
            auto* texture = m_attachmentTextures.at(i).get();
            auto vkTexture = static_cast<VulkanTexture*>(texture);

            attachments[i] = vkTexture->GetView();
            GFX_ASSERT(attachments[i]);
        }

        if (m_depthAttachentTexture)
        {
            auto vkTexture = static_cast<VulkanTexture*>(m_depthAttachentTexture.get());

            attachments.emplace_back(vkTexture->GetView());
            GFX_ASSERT(attachments.back());
        }

        vk::FramebufferCreateInfo framebufferInfo{};
        framebufferInfo.setRenderPass(m_renderPass);
        framebufferInfo.setAttachments(attachments);
        framebufferInfo.setWidth(m_desc.Width);
        framebufferInfo.setHeight(m_desc.Height);
        framebufferInfo.setLayers(1);

        m_framebuffer = vkDevice.createFramebuffer(framebufferInfo);
    }
}
