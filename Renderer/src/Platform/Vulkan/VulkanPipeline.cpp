//
// Created by stumi on 12/06/21.
//

#if GFX_API_VULKAN

    #include "VulkanPipeline.h"

    #include "VulkanCore.h"
    #include "VulkanShader.h"
    #include "VulkanFramebuffer.h"

    #include "GFX/Resources/VertexLayout.h"

    #include <utility>

namespace gfx
{
    static auto GetVulkanTopology(PrimitiveTopology topology) -> vk::PrimitiveTopology
    {
        switch (topology)
        {
            case PrimitiveTopology::ePoints: return vk::PrimitiveTopology::ePointList;
            case PrimitiveTopology::eLines: return vk::PrimitiveTopology::eLineList;
            case PrimitiveTopology::eTriangles: return vk::PrimitiveTopology::eTriangleList;
            case PrimitiveTopology::eNone: break;
        }
        return {};
    }

    static auto ShaderDataTypeToVulkanFormat(ShaderDataType type) -> vk::Format
    {
        switch (type)
        {
            case ShaderDataType::Float: return vk::Format::eR32Sfloat;
            case ShaderDataType::Float2: return vk::Format::eR32G32Sfloat;
            case ShaderDataType::Float3: return vk::Format::eR32G32B32Sfloat;
            case ShaderDataType::Float4: return vk::Format::eR32G32B32A32Sfloat;
        }
        return {};
    }

    Pipeline::Pipeline(PipelineDesc desc) : m_desc(std::move(desc)) { Invalidate(); }

    Pipeline::~Pipeline()
    {
        auto device = Vulkan::GetDevice();

        device.destroy(m_pipeline);
    }

    void Pipeline::Invalidate()
    {
        auto device = Vulkan::GetDevice();
        auto shader = m_desc.Shader;
        auto framebuffer = m_desc.Framebuffer;

        vk::PipelineLayoutCreateInfo layoutInfo{};
        // TODO: Get DescriptorSetLayouts from shader
        // TODO: Get PushConstantRanges from shader

        m_layout = device.createPipelineLayout(layoutInfo);

        vk::GraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.setLayout(m_layout);
        pipelineInfo.setRenderPass(framebuffer->GetRenderPass());

        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyState{};
        inputAssemblyState.setTopology(GetVulkanTopology(m_desc.Topology));

        vk::PipelineRasterizationStateCreateInfo rasterizationState{};
        rasterizationState.setPolygonMode(m_desc.Wireframe ? vk::PolygonMode::eLine : vk::PolygonMode::eFill);
        rasterizationState.setCullMode(m_desc.BackFaceCulling ? vk::CullModeFlagBits::eBack : vk::CullModeFlagBits::eNone);
        rasterizationState.setFrontFace(vk::FrontFace::eCounterClockwise);
        rasterizationState.setDepthClampEnable(false);
        rasterizationState.setRasterizerDiscardEnable(false);
        rasterizationState.setDepthBiasClamp(false);
        rasterizationState.setLineWidth(m_desc.LineWidth);  // Dynamic

        // TODO: Setup PipelineColorBlendAttachmentState
        auto colorAttachmentCount = /*framebuffer->IsSwapchainTarget() ?*/ 1 /*: framebuffer->GetAttachmentCount()*/;
        std::vector<vk::PipelineColorBlendAttachmentState> blendAttachmentStates(colorAttachmentCount);
        if (framebuffer->IsSwapchainTarget())
        {
            blendAttachmentStates[0].setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB |
                                                       vk::ColorComponentFlagBits::eA);
            blendAttachmentStates[0].setBlendEnable(true);
            blendAttachmentStates[0].setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha);
            blendAttachmentStates[0].setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha);
            blendAttachmentStates[0].setColorBlendOp(vk::BlendOp::eAdd);
            blendAttachmentStates[0].setAlphaBlendOp(vk::BlendOp::eAdd);
            blendAttachmentStates[0].setSrcAlphaBlendFactor(vk::BlendFactor::eOne);
            blendAttachmentStates[0].setDstAlphaBlendFactor(vk::BlendFactor::eZero);
        }
        else
        {
        }

        vk::PipelineColorBlendStateCreateInfo colorBlendState{};
        colorBlendState.setAttachments(blendAttachmentStates);

        // Both Viewport/Scissor are Dynamic
        vk::PipelineViewportStateCreateInfo viewportState{};
        viewportState.setViewportCount(1);
        viewportState.setScissorCount(1);

        std::vector<vk::DynamicState> dynamicStateEnables;
        dynamicStateEnables.push_back(vk::DynamicState::eViewport);
        dynamicStateEnables.push_back(vk::DynamicState::eScissor);
        if (m_desc.Topology == PrimitiveTopology::eLines /*|| m_desc.Topology == PrimitiveTopology::eLineStrip*/ || m_desc.Wireframe)
            dynamicStateEnables.push_back(vk::DynamicState::eLineWidth);

        vk::PipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.setDynamicStates(dynamicStateEnables);

        vk::PipelineDepthStencilStateCreateInfo depthStencilState{};
        depthStencilState.setDepthTestEnable(m_desc.DepthTest);
        depthStencilState.setDepthWriteEnable(m_desc.DepthWrite);
        depthStencilState.setDepthCompareOp(vk::CompareOp::eLessOrEqual);
        depthStencilState.setDepthBoundsTestEnable(false);
        depthStencilState.back.failOp = vk::StencilOp::eKeep;
        depthStencilState.back.passOp = vk::StencilOp::eKeep;
        depthStencilState.back.compareOp = vk::CompareOp::eAlways;
        depthStencilState.setStencilTestEnable(false);
        depthStencilState.front = depthStencilState.back;

        vk::PipelineMultisampleStateCreateInfo multisampleState{};
        multisampleState.setRasterizationSamples(vk::SampleCountFlagBits::e1);
        multisampleState.setPSampleMask(nullptr);

        auto& layout = m_desc.Layout;

        vk::VertexInputBindingDescription vertexInputBinding{};
        vertexInputBinding.binding = 0;
        vertexInputBinding.stride = layout.GetStride();
        vertexInputBinding.inputRate = vk::VertexInputRate::eVertex;

        std::vector<vk::VertexInputAttributeDescription> vertexAttributes(layout.GetElementCount());

        uint32_t location = 0;
        for (auto& element : layout)
        {
            vertexAttributes[location].binding = 0;
            vertexAttributes[location].location = location;
            vertexAttributes[location].format = ShaderDataTypeToVulkanFormat(element.Type);
            vertexAttributes[location].offset = element.Offset;

            location++;
        }

        vk::PipelineVertexInputStateCreateInfo vertexInputState{};
        vertexInputState.setVertexBindingDescriptions(vertexInputBinding);
        vertexInputState.setVertexAttributeDescriptions(vertexAttributes);

        const auto& shaderStages = shader->GetPipelineShaderStageCreateInfos();

        pipelineInfo.setStages(shaderStages);
        pipelineInfo.setPVertexInputState(&vertexInputState);
        pipelineInfo.setPInputAssemblyState(&inputAssemblyState);
        pipelineInfo.setPRasterizationState(&rasterizationState);
        pipelineInfo.setPColorBlendState(&colorBlendState);
        pipelineInfo.setPMultisampleState(&multisampleState);
        pipelineInfo.setPViewportState(&viewportState);
        pipelineInfo.setPDepthStencilState(&depthStencilState);
        pipelineInfo.setPDynamicState(&dynamicState);

        m_pipeline = device.createGraphicsPipeline({}, pipelineInfo).value;
    }

}  // namespace gfx

#endif