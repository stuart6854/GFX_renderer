//
// Created by stumi on 07/06/21.
//

#ifndef PERSONAL_RENDERER_VULKANDEVICECONTEXT_H
#define PERSONAL_RENDERER_VULKANDEVICECONTEXT_H

#include "VulkanCommandBuffer.h"
#include "VulkanBuffer.h"
#include "VulkanShader.h"
#include "VulkanTexture.h"

#include "GFX/Resources/ResourceDescriptions.h"

#include <memory>

namespace gfxOld
{
    class RenderContext;

    /**
     * Device Context is basically equal to a window.
     *
     * It manages window-side resources such as frame buffers, describes pixel formats of the window and frame buffer, etc.
     */
    class DeviceContext
    {
    public:
        auto CreateBuffer(BufferDesc desc) -> std::shared_ptr<Buffer>;
        auto CreateShader(const std::string& path) -> std::shared_ptr<Shader>;
        auto CreatePipeline(const PipelineDesc& desc) -> std::shared_ptr<Pipeline>;

        void Upload(Buffer* dst, const void* data);
        void Upload(Texture* texture);

        void Submit(CommandBuffer& cmdBuffer);

    private:
    private:
        /* Resources */
        std::vector<std::shared_ptr<Buffer>> m_buffers;
        std::vector<std::shared_ptr<Shader>> m_shaders;
        std::vector<std::shared_ptr<Pipeline>> m_pipelines;
    };
}  // namespace gfxOld

#endif  // PERSONAL_RENDERER_VULKANDEVICECONTEXT_H
