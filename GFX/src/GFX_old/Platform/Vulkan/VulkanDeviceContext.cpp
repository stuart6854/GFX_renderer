//
// Created by stumi on 07/06/21.
//

#ifdef GFX_API_VULKAN

    #include "VulkanDeviceContext.h"

    #include "VulkanCore.h"
    #include "VulkanRenderContext.h"

namespace gfxOld
{
    auto DeviceContext::CreateBuffer(BufferDesc desc) -> std::shared_ptr<Buffer>
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

        auto buffer = std::make_shared<Buffer>(desc);
        buffer->SetAPIResource(vkBuffer, allocation);

        m_buffers.push_back(buffer);
        return buffer;
    }

    auto DeviceContext::CreateShader(const std::string& path) -> std::shared_ptr<Shader>
    {
        auto shader = std::make_shared<Shader>(path);
        m_shaders.push_back(shader);
        return shader;
    }

    auto DeviceContext::CreatePipeline(const PipelineDesc& desc) -> std::shared_ptr<Pipeline>
    {
        auto pipeline = std::make_shared<Pipeline>(desc);
        m_pipelines.push_back(pipeline);
        return pipeline;
    }

    void DeviceContext::Upload(Buffer* dst, const void* data)
    {
        const auto size = dst->GetSize();

        BufferDesc stagingBufferDesc{ .Type = BufferType::eStaging, .Size = size };
        auto stagingBuffer = CreateBuffer(stagingBufferDesc);

        auto allocator = Vulkan::GetAllocator();

        auto* mapped = allocator.Map(stagingBuffer->GetAPIAllocation());
        std::memcpy(mapped, data, size);
        allocator.Unmap(stagingBuffer->GetAPIAllocation());

        CommandBuffer cmdBuffer;
        cmdBuffer.Begin();
        cmdBuffer.CopyBuffer(stagingBuffer->GetAPIBuffer(), dst->GetAPIBuffer(), size);
        cmdBuffer.End();

        Submit(cmdBuffer);

        // Wait for fence to signal command buffer has finished execution
        Vulkan::GetDevice().waitForFences(cmdBuffer.GetFence(), VK_TRUE, UINT64_MAX);
    }

    void DeviceContext::Upload(Texture* texture)
    {
        auto allocator = Vulkan::GetAllocator();

        auto imageData = texture->GetImageData();
        BufferDesc stagingBufferDesc{ .Type = BufferType::eStaging, .Size = imageData.Size };
        auto stagingBuffer = CreateBuffer(stagingBufferDesc);

        auto* mapped = allocator.Map(stagingBuffer->GetAPIAllocation());
        std::memcpy(mapped, imageData.Data, imageData.Size);
        allocator.Unmap(stagingBuffer->GetAPIAllocation());

        auto& textureDesc = texture->GetDesc();
        vk::BufferImageCopy copyRegion{};
        copyRegion.imageExtent.setWidth(textureDesc.Width);
        copyRegion.imageExtent.setHeight(textureDesc.Height);
        copyRegion.imageExtent.setDepth(textureDesc.Depth);
        copyRegion.imageSubresource.setAspectMask(vk::ImageAspectFlagBits::eColor);
        copyRegion.imageSubresource.setMipLevel(0);
        copyRegion.imageSubresource.setBaseArrayLayer(0);
        copyRegion.imageSubresource.setLayerCount(textureDesc.Layers);
        copyRegion.setBufferOffset(0);

        vk::ImageSubresourceRange imageRange{};
        imageRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
        imageRange.setBaseMipLevel(0);
        imageRange.setLevelCount(1);
        imageRange.setLayerCount(1);

        CommandBuffer cmdBuffer;
        cmdBuffer.Begin();
        cmdBuffer.ImageMemoryBarrier(texture->GetVulkanImage(),
                                     {},
                                     vk::AccessFlagBits::eTransferWrite,
                                     vk::ImageLayout::eUndefined,
                                     vk::ImageLayout::eTransferDstOptimal,
                                     vk::PipelineStageFlagBits::eHost,
                                     vk::PipelineStageFlagBits::eTransfer,
                                     imageRange);
        cmdBuffer.CopyBufferToImage(stagingBuffer->GetAPIBuffer(), texture->GetVulkanImage(), vk::ImageLayout::eTransferDstOptimal, copyRegion);
        cmdBuffer.ImageMemoryBarrier(texture->GetVulkanImage(),
                                     vk::AccessFlagBits::eTransferWrite,
                                     vk::AccessFlagBits::eShaderRead,
                                     vk::ImageLayout::eTransferDstOptimal,
                                     vk::ImageLayout::eShaderReadOnlyOptimal,
                                     vk::PipelineStageFlagBits::eTransfer,
                                     vk::PipelineStageFlagBits::eFragmentShader,
                                     imageRange);
        cmdBuffer.End();

        Submit(cmdBuffer);

        Vulkan::GetDevice().waitForFences(cmdBuffer.GetFence(), VK_TRUE, UINT64_MAX);
    }

    void DeviceContext::Submit(CommandBuffer& cmdBuffer)
    {
        auto apiBuffer = cmdBuffer.GetAPIResource();

        vk::SubmitInfo submitInfo{};
        submitInfo.setCommandBuffers(apiBuffer);

        auto graphicsQueue = Vulkan::GetGraphicsQueue();
        graphicsQueue.submit(submitInfo, cmdBuffer.GetFence());
    }

}  // namespace gfxOld

#endif
