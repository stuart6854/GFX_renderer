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
    DeviceContext::~DeviceContext()
    {
        auto device = Vulkan::GetDevice();
        device.destroy(m_swapchain);

        Vulkan::GetInstance().destroy(m_surface);
    }

    void DeviceContext::ProcessWindowChanges(ISurface& surface, uint32_t windowWidth, uint32_t windowHeight)
    {
        m_surface = surface.CreateSurface(Vulkan::GetInstance());

        auto result = Vulkan::GetPhysicalDevice().getSurfaceSupportKHR(Vulkan::GetGraphicsQueueFamily(), m_surface);

        auto physicalDevice = Vulkan::GetPhysicalDevice();
        auto device = Vulkan::GetDevice();

        auto capabilities = physicalDevice.getSurfaceCapabilitiesKHR(m_surface);
        auto surfaceFormat = Vulkan::ChooseSurfaceFormat(physicalDevice, m_surface);
        auto presentMode = Vulkan::ChoosePresentMode(physicalDevice, m_surface);
        auto extent = Vulkan::ChooseExtent(physicalDevice, m_surface, windowWidth, windowHeight);

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
        graphicsQueue.submit(submitInfo, GetCurrentFrame().RenderFence);
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

    auto DeviceContext::GetCurrentFrame() -> DeviceContext::Frame& { return m_frames.at(m_frameCounter % FRAME_OVERLAP); }

}  // namespace gfx

#endif
