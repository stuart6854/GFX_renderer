#include "Platform/Vulkan/VulkanSwapChain.h"

#include "GFX/Debug.h"
#include "GFX/Core/Window.h"
#include "VulkanBackend.h"
#include "VulkanCommandBuffer.h"

#include <GLFW/glfw3.h>

namespace gfx
{
    namespace Utils
    {
        auto ChooseSurfaceFormat(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface) -> vk::SurfaceFormatKHR
        {
            auto surfaceFormats = physicalDevice.getSurfaceFormatsKHR(surface);

            for (const auto& format : surfaceFormats)
            {
                if (format.format == vk::Format::eR8G8B8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
                {
                    return format;
                }
            }
            return surfaceFormats[0];
        }

        auto ChoosePresentMode(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface) -> vk::PresentModeKHR
        {
            auto presentModes = physicalDevice.getSurfacePresentModesKHR(surface);

            for (const auto& mode : presentModes)
            {
                if (mode == vk::PresentModeKHR::eMailbox)
                {
                    return mode;
                }
            }

            // Default to double buffering (VSync)
            return vk::PresentModeKHR::eFifo;
        }

        auto ChooseExtent(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, uint32_t width, uint32_t height) -> vk::Extent2D
        {
            auto capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);

            if (capabilities.currentExtent.width != UINT32_MAX)
            {
                return capabilities.currentExtent;
            }

            vk::Extent2D actualExtent = { width, height };

            // Use Min/Max to clamp the values between the allowed minimum and
            // maximum extents that are supported
            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    VulkanSwapChain::VulkanSwapChain(Window* window)
        : m_window(window)
    {
        //TODO: Assert glfwVulkanSupported();

        auto* backend = VulkanBackend::Get();
        auto& gpu = backend->GetPhysicalDevice();
        auto vkGpu = gpu.GetHandle();

        VkSurfaceKHR rawSurface = nullptr;
        glfwCreateWindowSurface(backend->GetInstance(), window->GetHandle(), nullptr, &rawSurface);
        m_surface = rawSurface;
        GFX_ASSERT(m_surface, "glfwCreateWindowSurface() failed!");

        vkGpu.getSurfaceSupportKHR(gpu.GetQueueFamilyIndices().Graphics, m_surface);

        CreateFrameResources();

        Recreate(m_window->GetWidth(), m_window->GetHeight());
    }

    VulkanSwapChain::~VulkanSwapChain()
    {
        Destroy();
    }

    void VulkanSwapChain::Recreate(uint32_t width, uint32_t height)
    {
        auto* backend = VulkanBackend::Get();
        auto& device = backend->GetDevice();
        auto vkDevice = device.GetHandle();
        auto& physicalDevice = backend->GetPhysicalDevice();
        auto vkPhysicalDevice = physicalDevice.GetHandle();

        backend->WaitIdle();

        const auto capabilities = vkPhysicalDevice.getSurfaceCapabilitiesKHR(m_surface);
        const auto surfaceFormat = Utils::ChooseSurfaceFormat(vkPhysicalDevice, m_surface);
        const auto presentMode = Utils::ChoosePresentMode(vkPhysicalDevice, m_surface);
        const auto extent = Utils::ChooseExtent(vkPhysicalDevice, m_surface, width, height);

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

        const auto newSwapChain = vkDevice.createSwapchainKHR(createInfo);
        GFX_ASSERT(newSwapChain, "createSwapchainKHR() failed!");
        if (m_swapChain)
        {
            DestroySwapChainResources();
            vkDevice.destroy(m_swapChain);
        }
        m_swapChain = newSwapChain;

        m_width = extent.width;
        m_height = extent.height;
        m_format = surfaceFormat.format;

        m_images = vkDevice.getSwapchainImagesKHR(m_swapChain);

        CreateRenderPass();
        CreateDepthStencil();
        CreateImageResources();
    }

    void VulkanSwapChain::NewFrame()
    {
        auto* backend = VulkanBackend::Get();
        auto& device = backend->GetDevice();

        auto& frame = GetCurrentFrame();
        // Wait until GPU has finished rendering the last frame
        if (frame.RenderFence)
            device.WaitForFence(frame.RenderFence);

        // Vulkan::ResetDescriptorPool(GetFrameIndex());
        device.ResetDescriptorPool(m_frameIndex);

        // Request image from swapchain
        m_imageIndex = device.AcquireNextImage(m_swapChain, frame.PresentComplete);
    }

    void VulkanSwapChain::Present(CommandBuffer* cmdBuffer)
    {
        auto* backend = VulkanBackend::Get();
        auto& device = backend->GetDevice();
        auto* vkCmdBuffer = static_cast<VulkanCommandBuffer*>(cmdBuffer);

        auto cmdBufferHandle = vkCmdBuffer->GetHandle();
        std::vector<vk::PipelineStageFlags> waitStages = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

        vk::SubmitInfo submitInfo{};
        submitInfo.setCommandBuffers(cmdBufferHandle);
        submitInfo.setWaitSemaphores(GetCurrentFrame().PresentComplete);
        submitInfo.setSignalSemaphores(GetCurrentFrame().RenderComplete);
        submitInfo.setWaitDstStageMask(waitStages);

        GetCurrentFrame().RenderFence = vkCmdBuffer->GetFence();
        device.GetGraphicsQueue().submit(submitInfo, GetCurrentFrame().RenderFence);

        vk::PresentInfoKHR presentInfo{};
        presentInfo.setSwapchains(m_swapChain);
        presentInfo.setImageIndices(m_imageIndex);
        presentInfo.setWaitSemaphores(GetCurrentFrame().RenderComplete);

        void(device.GetGraphicsQueue().presentKHR(presentInfo));

        m_frameIndex = (m_frameIndex + 1) % Config::FramesInFlight;
    }

    void VulkanSwapChain::CreateFrameResources()
    {
        auto* backend = VulkanBackend::Get();
        auto& device = backend->GetDevice();
        auto vkDevice = device.GetHandle();

        // vk::FenceCreateInfo fenceInfo{ vk::FenceCreateFlagBits::eSignaled };
        const vk::SemaphoreCreateInfo semaphoreInfo{};

        for (int i = 0; i < Config::FramesInFlight; ++i)
        {
            auto& frame = m_frames.at(i);

            //            frame.RenderFence = device.createFence(fenceInfo);
            frame.PresentComplete = vkDevice.createSemaphore(semaphoreInfo);
            frame.RenderComplete = vkDevice.createSemaphore(semaphoreInfo);
        }
    }

    void VulkanSwapChain::CreateRenderPass()
    {
        auto depthFormat = VulkanBackend::Get()->GetPhysicalDevice().GetDepthFormat();

        /* Attachment Descriptions */
        vk::AttachmentDescription colorAttachment{};
        colorAttachment.setFormat(m_format);
        colorAttachment.setSamples(vk::SampleCountFlagBits::e1);
        colorAttachment.setLoadOp(vk::AttachmentLoadOp::eClear);
        colorAttachment.setStoreOp(vk::AttachmentStoreOp::eStore);
        colorAttachment.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
        colorAttachment.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
        colorAttachment.setInitialLayout(vk::ImageLayout::eUndefined);
        colorAttachment.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

        vk::AttachmentDescription depthAttachment{};
        depthAttachment.setFormat(depthFormat);
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

        auto* backend = VulkanBackend::Get();
        auto& device = backend->GetDevice();
        auto vkDevice = device.GetHandle();

        m_renderPass = vkDevice.createRenderPass(renderPassInfo);
    }

    void VulkanSwapChain::CreateDepthStencil()
    {
        auto* backend = VulkanBackend::Get();
        auto& device = backend->GetDevice();
        auto vkDevice = device.GetHandle();
        auto& allocator = backend->GetAllocator();

        const vk::Format depthFormat = VulkanBackend::Get()->GetPhysicalDevice().GetDepthFormat();

        // TODO: Replace with VulkanImage?
        vk::ImageCreateInfo imageInfo{};
        imageInfo.imageType = vk::ImageType::e2D;
        imageInfo.format = depthFormat;
        imageInfo.extent.width = m_width;
        imageInfo.extent.height = m_height;
        imageInfo.extent.depth = 1;
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

        m_depthStencil.View = vkDevice.createImageView(imageViewInfo);
    }

    void VulkanSwapChain::CreateImageResources()
    {
        auto* backend = VulkanBackend::Get();
        auto& device = backend->GetDevice();
        auto vkDevice = device.GetHandle();

        m_images = vkDevice.getSwapchainImagesKHR(m_swapChain);
        m_imageViews.resize(m_images.size());
        m_framebuffers.resize(m_images.size());

        vk::ImageViewCreateInfo viewInfo{};
        viewInfo.setViewType(vk::ImageViewType::e2D);
        viewInfo.setFormat(m_format);
        viewInfo.setComponents({});
        viewInfo.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
        viewInfo.subresourceRange.setBaseMipLevel(0);
        viewInfo.subresourceRange.setLayerCount(1);
        viewInfo.subresourceRange.setBaseArrayLayer(0);
        viewInfo.subresourceRange.setLevelCount(1);

        vk::FramebufferCreateInfo framebufferInfo{};
        framebufferInfo.renderPass = m_renderPass;
        framebufferInfo.width = m_width;
        framebufferInfo.height = m_height;
        framebufferInfo.layers = 1;

        for (int i = 0; i < m_images.size(); i++)
        {
            viewInfo.setImage(m_images.at(i));
            m_imageViews[i] = vkDevice.createImageView(viewInfo);

            std::vector<vk::ImageView> attachments = { m_imageViews[i], m_depthStencil.View };
            framebufferInfo.setAttachments(attachments);
            m_framebuffers[i] = vkDevice.createFramebuffer(framebufferInfo);
        }
    }

    void VulkanSwapChain::DestroySwapChainResources()
    {
        auto* backend = VulkanBackend::Get();
        auto& device = backend->GetDevice();
        auto vkDevice = device.GetHandle();
        auto& allocator = backend->GetAllocator();

        vkDevice.destroy(m_renderPass);
        for (int i = 0; i < m_images.size(); i++)
        {
            vkDevice.destroy(m_framebuffers[i]);
            vkDevice.destroy(m_imageViews[i]);
        }
        m_images.clear();
        m_imageViews.clear();
        m_framebuffers.clear();

        vkDevice.destroy(m_depthStencil.View);
        allocator.Free(m_depthStencil.Image, m_depthStencil.Allocation);
    }

    void VulkanSwapChain::Destroy()
    {
        DestroySwapChainResources();

        auto* backend = VulkanBackend::Get();
        auto& device = backend->GetDevice();
        auto vkDevice = device.GetHandle();

        for (uint32_t i = 0; i < Config::FramesInFlight; ++i)
        {
            auto& frame = m_frames.at(i);

            vkDevice.destroy(frame.RenderFence);
            vkDevice.destroy(frame.PresentComplete);
            vkDevice.destroy(frame.RenderComplete);
        }

        vkDevice.destroy(m_swapChain);
        backend->GetInstance().destroy(m_surface);
    }
}
