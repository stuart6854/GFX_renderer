#pragma once

#include "GFX/Resources/Pipeline.h"

#include <vulkan/vulkan.hpp>

namespace gfx
{
    class VulkanPipeline : public Pipeline
    {
    public:
        VulkanPipeline(const PipelineDesc& desc);
        ~VulkanPipeline();

        auto GetPipelineHandle() -> vk::Pipeline { return m_pipeline; }
        auto GetLayoutHandle() -> vk::PipelineLayout { return m_layout; }

    private:
    private:
        PipelineDesc m_desc;

        vk::PipelineLayout m_layout;
        vk::Pipeline m_pipeline;
    };
}
