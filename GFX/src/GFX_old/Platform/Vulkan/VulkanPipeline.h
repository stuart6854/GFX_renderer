//
// Created by stumi on 12/06/21.
//

#ifndef PERSONAL_RENDERER_VULKANPIPELINE_H
#define PERSONAL_RENDERER_VULKANPIPELINE_H

#include "GFX/Resources/ResourceDescriptions.h"

#include <vulkan/vulkan.hpp>

namespace gfxOld
{
    class Pipeline
    {
    public:
        Pipeline(PipelineDesc desc);
        ~Pipeline();

        void Invalidate();

        auto GetAPIPipeline() -> vk::Pipeline { return m_pipeline; }
        auto GetAPIPipelineLayout() -> vk::PipelineLayout { return m_layout; }

    private:
        PipelineDesc m_desc;

        vk::PipelineLayout m_layout;
        vk::Pipeline m_pipeline;
    };

}  // namespace gfxOld

#endif  // PERSONAL_RENDERER_VULKANPIPELINE_H
