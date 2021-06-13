//
// Created by stumi on 07/06/21.
//

#ifdef GFX_API_VULKAN

    #include "VulkanDeviceContext.h"

    #include "VulkanCore.h"
    #include "VulkanRenderContext.h"
    #include "VulkanUtils.h"

namespace gfx
{
    DeviceContext::DeviceContext() { CreateFramesResources(); }

    DeviceContext::~DeviceContext() { Destroy(); }

    void DeviceContext::ProcessWindowChanges(ISurface& surface, uint32_t windowWidth, uint32_t windowHeight)
    {
        m_surface = surface.CreateSurface(Vulkan::GetInstance());

        auto result = Vulkan::GetPhysicalDevice().getSurfaceSupportKHR(Vulkan::GetGraphicsQueueFamily(), m_surface);

        CreateSwapchain(windowWidth, windowHeight);

        DestroySwapchainResources();

        CreateDepthStencil();
        CreateRenderPass();
        CreateImageResources();
        CreateFramesResources();
    }

    auto DeviceContext::CreateBuffer(BufferDesc desc) -> Buffer
    {
        auto allocator = Vulkan::GetAllocator();

        auto usage = BufferTypeToVulkan(desc.Type);
        if (desc.Type != BufferType::eStaging) usage |= vk::BufferUsageFlagBits::eTransferDst;

        vk::BufferCreateInfo bufferInfo{};
        bufferInfo.setSize(desc.Size);
        bufferInfo.setUsage(usage);

        auto memoryType = (desc.Type == BufferType::eStaging) ? VMA_MEMORY_USAGE_CPU_ONLY : VMA_MEMORY_USAGE_GPU_ONLY;

        vk::Buffer vkBuffer;
        VmaAllocation allocation;
        allocator.Allocate(bufferInfo, memoryType, &vkBuffer, &allocation);

        Buffer buffer(desc);
        buffer.SetAPIResource(vkBuffer, allocation);

        return buffer;
    }

    auto DeviceContext::CreateShader() -> Shader { return Shader(""); }

    void DeviceContext::Upload(Buffer& dst, const void* data)
    {
        const auto size = dst.GetSize();

        BufferDesc stagingBufferDesc{ .Type = BufferType::eStaging, .Size = size };
        auto stagingBuffer = CreateBuffer(stagingBufferDesc);

        auto allocator = Vulkan::GetAllocator();

        auto* mapped = allocator.Map(stagingBuffer.GetAPIAllocation());
        std::memcpy(mapped, data, size);
        allocator.Unmap(stagingBuffer.GetAPIAllocation());

        CommandBuffer cmdBuffer;
        cmdBuffer.Begin();
        cmdBuffer.CopyBuffer(stagingBuffer.GetAPIBuffer(), dst.GetAPIBuffer(), size);
        cmdBuffer.End();

        Submit(cmdBuffer);

        // Wait for fence to signal command buffer has finished execution
        Vulkan::GetDevice().waitForFences(cmdBuffer.GetFence(), VK_TRUE, UINT64_MAX);
    }

    void DeviceContext::NewFrame()
    {
        auto device = Vulkan::GetDevice();
        auto& frame = GetCurrentFrame();

        // Wait until GPU has finished rendering the last frame
        if (frame.RenderFence) device.waitForFences(frame.RenderFence, VK_TRUE, UINT64_MAX);
        //        device.resetFences(frame.RenderFence);

        // Request image from swapchain
        m_swapchainImageIndex = device.acquireNextImageKHR(m_swapchain, UINT64_MAX, frame.PresentComplete, {}).value;
    }

    void DeviceContext::Submit(RenderContext& context)
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

        auto graphicsQueue = Vulkan::GetGraphicsQueue();
        // RenderComplete fence will now block until the graphics commands finish execution
        GetCurrentFrame().RenderFence = context.GetCommandBuffer().GetFence();
        graphicsQueue.submit(submitInfo, GetCurrentFrame().RenderFence);

        context.NextCommandBuffer();
    }

    void DeviceContext::Submit(CommandBuffer& cmdBuffer)
    {
        auto apiBuffer = cmdBuffer.GetAPIResource();

        vk::SubmitInfo submitInfo{};
        submitInfo.setCommandBuffers(apiBuffer);

        auto graphicsQueue = Vulkan::GetGraphicsQueue();
        graphicsQueue.submit(submitInfo, cmdBuffer.GetFence());
    }

    void DeviceContext::Present()
    {
        vk::PresentInfoKHR presentInfo{};
        presentInfo.setSwapchains(m_swapchain);
        presentInfo.setImageIndices(m_swapchainImageIndex);
        // We will wait on the RenderComplete semaphore before presenting
        presentInfo.setWaitSemaphores(GetCurrentFrame().RenderComplete);

        auto graphicsQueue = Vulkan::GetGraphicsQueue();
        graphicsQueue.presentKHR(presentInfo);

        m_frameCounter++;
    }

    auto DeviceContext::GetFramebuffer() -> Framebuffer
    {
        auto framebuffer = m_swapchainFramebuffers.at(m_swapchainImageIndex);
        vk::Extent2D extent = { m_swapchainWidth, m_swapchainHeight };
        return Framebuffer(framebuffer, m_swapchainRenderPass, extent);
    }

    auto DeviceContext::GetCurrentFrame() -> DeviceContext::Frame& { return m_frames.at(m_frameCounter % FRAME_OVERLAP); }

    void DeviceContext::CreateSwapchain(uint32_t width, uint32_t height)
    {
        auto physicalDevice = Vulkan::GetPhysicalDevice();
        auto device = Vulkan::GetDevice();

        auto capabilities = physicalDevice.getSurfaceCapabilitiesKHR(m_surface);
        auto surfaceFormat = Vulkan::ChooseSurfaceFormat(physicalDevice, m_surface);
        auto presentMode = Vulkan::ChoosePresentMode(physicalDevice, m_surface);
        auto extent = Vulkan::ChooseExtent(physicalDevice, m_surface, height, height);

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
        createInfo.oldSwapchain = m_swapchain;

        auto newSwapchain = device.createSwapchainKHR(createInfo);
        if (m_swapchain)
        {
            device.destroy(m_swapchain);
        }
        m_swapchain = newSwapchain;

        m_swapchainWidth = extent.width;
        m_swapchainHeight = extent.height;
        m_swapchainFormat = surfaceFormat.format;

        m_swapchainImages = device.getSwapchainImagesKHR(m_swapchain);
    }

    void DeviceContext::CreateDepthStencil()
    {
        auto device = Vulkan::GetDevice();
        auto& allocator = Vulkan::GetAllocator();

        vk::Format depthFormat = Vulkan::GetDepthFormat();

        vk::ImageCreateInfo imageInfo{};
        imageInfo.imageType = vk::ImageType::e2D;
        imageInfo.format = depthFormat;
        imageInfo.extent.width = m_swapchainWidth;
        imageInfo.extent.height = m_swapchainHeight;
        imageInfo.extent.depth = 1.0f;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.samples = vk::SampleCountFlagBits::e1;
        imageInfo.tiling = vk::ImageTiling::eOptimal;
        imageInfo.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eTransferSrc;

        allocator.Allocate(imageInfo, VMA_MEMORY_USAGE_GPU_ONLY, &m_depthStencil.Image, &m_depthStencil.Allocation);

        vk::ImageViewCreateInfo imageViewInfo{};
        imageViewInfo.viewType = vk::ImageViewType::e2D;
        imageViewInfo.image = m_depthStencil.Image;
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

        m_depthStencil.View = device.createImageView(imageViewInfo);
    }

    void DeviceContext::CreateRenderPass()
    {
        /* Attachment Descriptions */
        vk::AttachmentDescription colorAttachment{};
        colorAttachment.format = m_swapchainFormat;
        colorAttachment.samples = vk::SampleCountFlagBits::e1;
        colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
        colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
        colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

        vk::AttachmentDescription depthAttachment{};
        depthAttachment.format = Vulkan::GetDepthFormat();
        depthAttachment.samples = vk::SampleCountFlagBits::e1;
        depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        depthAttachment.storeOp = vk::AttachmentStoreOp::eDontCare;
        depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
        depthAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

        /* Subpasses & Attachment References */
        vk::AttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

        vk::AttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

        vk::SubpassDescription subpass{};
        subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;

        /* Render Pass */
        auto attachments = { colorAttachment, depthAttachment };

        vk::RenderPassCreateInfo renderPassInfo{};
        renderPassInfo.attachmentCount = attachments.size();
        renderPassInfo.setAttachments(attachments);
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;

        vk::SubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        dependency.srcAccessMask = {};
        dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        m_swapchainRenderPass = Vulkan::GetDevice().createRenderPass(renderPassInfo);
    }

    void DeviceContext::CreateImageResources()
    {
        auto device = Vulkan::GetDevice();

        m_swapchainImageViews.resize(m_swapchainImages.size());
        vk::ImageViewCreateInfo viewInfo{};
        viewInfo.setViewType(vk::ImageViewType::e2D);
        viewInfo.setFormat(m_swapchainFormat);
        viewInfo.setComponents({});
        viewInfo.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
        viewInfo.subresourceRange.setBaseMipLevel(0);
        viewInfo.subresourceRange.setLayerCount(1);
        viewInfo.subresourceRange.setBaseArrayLayer(0);
        viewInfo.subresourceRange.setLevelCount(1);

        vk::FramebufferCreateInfo framebufferInfo{};
        framebufferInfo.renderPass = m_swapchainRenderPass;
        framebufferInfo.width = m_swapchainWidth;
        framebufferInfo.height = m_swapchainHeight;
        framebufferInfo.layers = 1;

        m_swapchainFramebuffers.resize(m_swapchainImages.size());
        for (int i = 0; i < m_swapchainImages.size(); i++)
        {
            viewInfo.setImage(m_swapchainImages[i]);
            m_swapchainImageViews[i] = device.createImageView(viewInfo);

            std::vector<vk::ImageView> attachments = { m_swapchainImageViews[i], m_depthStencil.View };
            framebufferInfo.setAttachments(attachments);

            m_swapchainFramebuffers[i] = device.createFramebuffer(framebufferInfo);
        }
    }

    void DeviceContext::CreateFramesResources()
    {
        auto device = Vulkan::GetDevice();

        vk::FenceCreateInfo fenceInfo{ vk::FenceCreateFlagBits::eSignaled };
        vk::SemaphoreCreateInfo semaphoreInfo{};

        for (int i = 0; i < FRAME_OVERLAP; ++i)
        {
            auto& frame = m_frames.at(i);

            //            frame.RenderFence = device.createFence(fenceInfo);
            frame.PresentComplete = device.createSemaphore(semaphoreInfo);
            frame.RenderComplete = device.createSemaphore(semaphoreInfo);
        }
    }

    void DeviceContext::DestroySwapchainResources()
    {
        auto device = Vulkan::GetDevice();
        auto& allocator = Vulkan::GetAllocator();

        device.destroy(m_swapchainRenderPass);
        for (auto& m_swapchainImageView : m_swapchainImageViews)
        {
            device.destroy(m_swapchainImageView);
        }
        for (auto& m_swapchainFramebuffer : m_swapchainFramebuffers)
        {
            device.destroy(m_swapchainFramebuffer);
        }
        m_swapchainImageViews.clear();
        m_swapchainFramebuffers.clear();

        device.destroy(m_depthStencil.View);
        allocator.Free(m_depthStencil.Image, m_depthStencil.Allocation);
    }

    void DeviceContext::Destroy()
    {
        DestroySwapchainResources();

        auto device = Vulkan::GetDevice();

        device.destroy(m_swapchain);
        for (int i = 0; i < FRAME_OVERLAP; ++i)
        {
            auto& frame = m_frames.at(i);

            device.destroy(frame.RenderFence);
            device.destroy(frame.PresentComplete);
            device.destroy(frame.RenderComplete);
        }

        Vulkan::GetInstance().destroy(m_surface);
    }

}  // namespace gfx

#endif
