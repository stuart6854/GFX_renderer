#ifdef GFX_API_VULKAN

    #include "VulkanRenderSurface.h"

    #include "VulkanCore.h"
    #include "VulkanUtils.h"
    #include "VulkanRenderContext.h"

    #include "GFX/Debug.h"
    #include "GFX/IWindowSurface.h"

namespace gfx
{
    RenderSurface::RenderSurface(IWindowSurface& windowSurface)
    {
        m_surface = windowSurface.CreateSurface(Vulkan::GetInstance());

        const auto result = Vulkan::GetPhysicalDevice().getSurfaceSupportKHR(Vulkan::GetGraphicsQueueFamily(), m_surface);
        GFX_ASSERT(result, "Graphics queue does not support presenting to this surface!");

        CreateFrameResources();

        CreateSwapChain(windowSurface.GetSurfaceWidth(), windowSurface.GetSurfaceHeight());
    }

    RenderSurface::~RenderSurface() { Destroy(); }

    void RenderSurface::Resize(const uint32_t width, const uint32_t height) { CreateSwapChain(width, height); }

    void RenderSurface::NewFrame()
    {
        const auto device = Vulkan::GetDevice();
        const auto& frame = GetCurrentFrame();

        // Wait until GPU has finished rendering the last frame
        if (frame.RenderFence) device.waitForFences(frame.RenderFence, VK_TRUE, UINT64_MAX);

        Vulkan::ResetDescriptorPool(GetFrameIndex());

        // Request image from swapchain
        m_swapChainImageIndex = device.acquireNextImageKHR(m_swapChain, UINT64_MAX, frame.PresentComplete, {}).value;
    }

    void RenderSurface::Submit(RenderContext& context)
    {
        auto cmdBuffer = context.GetCommandBuffer().GetAPIResource();

        vk::SubmitInfo submitInfo{};
        submitInfo.setCommandBuffers(cmdBuffer);

        std::vector<vk::PipelineStageFlags> waitStages = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
        submitInfo.setWaitDstStageMask(waitStages);

        // We want to wait on the PresentComplete semaphore, as that will be signaled when the swapchain is ready
        submitInfo.setWaitSemaphores(GetCurrentFrame().PresentComplete);

        // We will signal the RenderComplete semaphore, to signal that rendering has finished
        submitInfo.setSignalSemaphores(GetCurrentFrame().RenderComplete);

        const auto graphicsQueue = Vulkan::GetGraphicsQueue();
        // RenderComplete fence will now block until the graphics commands finish execution
        GetCurrentFrame().RenderFence = context.GetCommandBuffer().GetFence();
        graphicsQueue.submit(submitInfo, GetCurrentFrame().RenderFence);

        context.NextCommandBuffer();
    }

    void RenderSurface::Present()
    {
        vk::PresentInfoKHR presentInfo{};
        presentInfo.setSwapchains(m_swapChain);
        presentInfo.setImageIndices(m_swapChainImageIndex);
        // We will wait on the RenderComplete semaphore before presenting
        presentInfo.setWaitSemaphores(GetCurrentFrame().RenderComplete);

        const auto graphicsQueue = Vulkan::GetGraphicsQueue();
        graphicsQueue.presentKHR(presentInfo);

        m_frameIndex = (m_frameIndex + 1) % Config::FramesInFlight;
    }

    void RenderSurface::CreateSwapChain(const uint32_t width, const uint32_t height)
    {
        const auto physicalDevice = Vulkan::GetPhysicalDevice();
        const auto device = Vulkan::GetDevice();

        const auto capabilities = physicalDevice.getSurfaceCapabilitiesKHR(m_surface);
        const auto surfaceFormat = Vulkan::ChooseSurfaceFormat(physicalDevice, m_surface);
        const auto presentMode = Vulkan::ChoosePresentMode(physicalDevice, m_surface);
        const auto extent = Vulkan::ChooseExtent(physicalDevice, m_surface, width, height);

        auto imageCount = capabilities.minImageCount + 1;
        if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
        {
            imageCount = capabilities.maxImageCount;
        }

        // Create swapchain
        vk::SwapchainCreateInfoKHR createInfo{};
        createInfo.surface = m_surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
        createInfo.imageSharingMode = vk::SharingMode::eExclusive;
        createInfo.preTransform = capabilities.currentTransform;
        createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = m_swapChain;

        const auto newSwapChain = device.createSwapchainKHR(createInfo);
        if (m_swapChain)
        {
            DestroySwapChainResources();
            device.destroy(m_swapChain);
        }
        m_swapChain = newSwapChain;

        m_swapChainWidth = extent.width;
        m_swapChainHeight = extent.height;
        m_swapChainFormat = surfaceFormat.format;

        CreateRenderPass();
        CreateDepthStencil();
        CreateImageResources(device.getSwapchainImagesKHR(m_swapChain));
    }

    void RenderSurface::CreateFrameResources()
    {
        const auto device = Vulkan::GetDevice();

        // vk::FenceCreateInfo fenceInfo{ vk::FenceCreateFlagBits::eSignaled };
        const vk::SemaphoreCreateInfo semaphoreInfo{};

        for (int i = 0; i < Config::FramesInFlight; ++i)
        {
            auto& frame = m_frames.at(i);

            //            frame.RenderFence = device.createFence(fenceInfo);
            frame.PresentComplete = device.createSemaphore(semaphoreInfo);
            frame.RenderComplete = device.createSemaphore(semaphoreInfo);
        }
    }

    void RenderSurface::CreateRenderPass()
    {
        /* Attachment Descriptions */
        vk::AttachmentDescription colorAttachment{};
        colorAttachment.setFormat(m_swapChainFormat);
        colorAttachment.setSamples(vk::SampleCountFlagBits::e1);
        colorAttachment.setLoadOp(vk::AttachmentLoadOp::eClear);
        colorAttachment.setStoreOp(vk::AttachmentStoreOp::eStore);
        colorAttachment.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
        colorAttachment.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
        colorAttachment.setInitialLayout(vk::ImageLayout::eUndefined);
        colorAttachment.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

        vk::AttachmentDescription depthAttachment{};
        depthAttachment.setFormat(Vulkan::GetDepthFormat());
        depthAttachment.setSamples(vk::SampleCountFlagBits::e1);
        depthAttachment.setLoadOp(vk::AttachmentLoadOp::eClear);
        depthAttachment.setStoreOp(vk::AttachmentStoreOp::eDontCare);
        depthAttachment.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
        depthAttachment.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
        depthAttachment.setInitialLayout(vk::ImageLayout::eUndefined);
        depthAttachment.setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

        /* SubPasses & Attachment References */
        vk::AttachmentReference colorAttachmentRef{};
        colorAttachmentRef.setAttachment(0);
        colorAttachmentRef.setLayout(vk::ImageLayout::eColorAttachmentOptimal);

        vk::AttachmentReference depthAttachmentRef{};
        depthAttachmentRef.setAttachment(1);
        depthAttachmentRef.setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

        vk::SubpassDescription subPass{};
        subPass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);
        subPass.setColorAttachments(colorAttachmentRef);
        subPass.setPDepthStencilAttachment(&depthAttachmentRef);

        /* Render Pass */
        auto attachments = { colorAttachment, depthAttachment };

        vk::RenderPassCreateInfo renderPassInfo{};
        renderPassInfo.setAttachments(attachments);
        renderPassInfo.setSubpasses(subPass);

        vk::SubpassDependency dependency{};
        dependency.setSrcSubpass(VK_SUBPASS_EXTERNAL);
        dependency.setDstSubpass(0);
        dependency.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
        dependency.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
        dependency.setSrcAccessMask({});
        dependency.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);
        renderPassInfo.setDependencies(dependency);

        m_renderPass = Vulkan::GetDevice().createRenderPass(renderPassInfo);
    }

    void RenderSurface::CreateDepthStencil()
    {
        const auto device = Vulkan::GetDevice();
        auto& allocator = Vulkan::GetAllocator();

        const vk::Format depthFormat = Vulkan::GetDepthFormat();

        // TODO: Replace with VulkanImage?
        vk::ImageCreateInfo imageInfo{};
        imageInfo.imageType = vk::ImageType::e2D;
        imageInfo.format = depthFormat;
        imageInfo.extent.width = m_swapChainWidth;
        imageInfo.extent.height = m_swapChainHeight;
        imageInfo.extent.depth = 1.0f;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.samples = vk::SampleCountFlagBits::e1;
        imageInfo.tiling = vk::ImageTiling::eOptimal;
        imageInfo.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eTransferSrc;

        allocator.Allocate(imageInfo, VMA_MEMORY_USAGE_GPU_ONLY, &m_swapChainDepthStencil.Image, &m_swapChainDepthStencil.Allocation);

        vk::ImageViewCreateInfo imageViewInfo{};
        imageViewInfo.viewType = vk::ImageViewType::e2D;
        imageViewInfo.image = m_swapChainDepthStencil.Image;
        imageViewInfo.format = depthFormat;
        imageViewInfo.subresourceRange.baseMipLevel = 0;
        imageViewInfo.subresourceRange.levelCount = 1;
        imageViewInfo.subresourceRange.baseArrayLayer = 0;
        imageViewInfo.subresourceRange.layerCount = 1;
        imageViewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
        if (depthFormat >= vk::Format::eD16UnormS8Uint)
        {
            imageViewInfo.subresourceRange.aspectMask |= vk::ImageAspectFlagBits::eStencil;
        }

        m_swapChainDepthStencil.View = device.createImageView(imageViewInfo);
    }

    void RenderSurface::CreateImageResources(const std::vector<vk::Image>& swapChainImages)
    {
        const auto device = Vulkan::GetDevice();

        m_swapChainImages.resize(swapChainImages.size());

        vk::ImageViewCreateInfo viewInfo{};
        viewInfo.setViewType(vk::ImageViewType::e2D);
        viewInfo.setFormat(m_swapChainFormat);
        viewInfo.setComponents({});
        viewInfo.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
        viewInfo.subresourceRange.setBaseMipLevel(0);
        viewInfo.subresourceRange.setLayerCount(1);
        viewInfo.subresourceRange.setBaseArrayLayer(0);
        viewInfo.subresourceRange.setLevelCount(1);

        vk::FramebufferCreateInfo framebufferInfo{};
        framebufferInfo.renderPass = m_renderPass;
        framebufferInfo.width = m_swapChainWidth;
        framebufferInfo.height = m_swapChainHeight;
        framebufferInfo.layers = 1;

        for (int i = 0; i < m_swapChainImages.size(); i++)
        {
            auto& swapChainImage = m_swapChainImages.at(i);
            swapChainImage.Image = swapChainImages.at(i);

            viewInfo.setImage(swapChainImage.Image);
            swapChainImage.View = device.createImageView(viewInfo);

            std::vector<vk::ImageView> attachments = { swapChainImage.View, m_swapChainDepthStencil.View };
            framebufferInfo.setAttachments(attachments);

            swapChainImage.Framebuffer = device.createFramebuffer(framebufferInfo);
        }
    }

    void RenderSurface::DestroySwapChainResources()
    {
        const auto device = Vulkan::GetDevice();
        auto& allocator = Vulkan::GetAllocator();

        device.destroy(m_renderPass);
        for (auto& swapChainImage : m_swapChainImages)
        {
            device.destroy(swapChainImage.Framebuffer);
            device.destroy(swapChainImage.View);
        }
        m_swapChainImages.clear();

        device.destroy(m_swapChainDepthStencil.View);
        allocator.Free(m_swapChainDepthStencil.Image, m_swapChainDepthStencil.Allocation);
    }

    void RenderSurface::Destroy()
    {
        DestroySwapChainResources();

        const auto device = Vulkan::GetDevice();

        for (uint32_t i = 0; i < Config::FramesInFlight; ++i)
        {
            auto& frame = m_frames.at(i);

            device.destroy(frame.RenderFence);
            device.destroy(frame.PresentComplete);
            device.destroy(frame.RenderComplete);
        }

        device.destroy(m_swapChain);
        Vulkan::GetInstance().destroy(m_surface);
    }

}  // namespace gfx

#endif