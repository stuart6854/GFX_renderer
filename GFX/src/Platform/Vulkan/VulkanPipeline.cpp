#include "VulkanPipeline.h"

#include "VulkanBackend.h"
#include "VulkanDevice.h"
#include "VulkanFramebuffer.h"
#include "VulkanShader.h"

#include <vector>

namespace gfx
{
    namespace Utils
    {
        auto ToVulkanTopology(PrimitiveTopology topology) -> vk::PrimitiveTopology
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

        auto ToVulkanCullMode(const FaceCullMode mode) -> vk::CullModeFlags
        {
            switch (mode)
            {
                case FaceCullMode::eBack: return vk::CullModeFlagBits::eBack;
                case FaceCullMode::eFront: return vk::CullModeFlagBits::eFront;
                case FaceCullMode::eBoth: return vk::CullModeFlagBits::eFrontAndBack;
                case FaceCullMode::eNone:
                default: break;
            }
            return {};
        }

        auto ToVulkanFormat(ShaderDataType type) -> vk::Format
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
    }

    VulkanPipeline::VulkanPipeline(const PipelineDesc& desc)
        : m_desc(desc)
    {
        auto backend = VulkanBackend::Get();
        auto vkDevice = backend->GetDevice().GetHandle();

        auto* vkFramebuffer = static_cast<VulkanFramebuffer*>(desc.Framebuffer);
        auto* vkShader = static_cast<VulkanShader*>(desc.Shader);

        const auto& pushConstantRanges = vkShader->GetPushConstantRanges();

        std::vector<vk::PushConstantRange> vkPushConstantRanges(pushConstantRanges.size());
        for (int i = 0; i < pushConstantRanges.size(); i++)
        {
            const auto& pushConstantRange = pushConstantRanges[i];
            auto& vkPushConstantRange = vkPushConstantRanges[i];

            vkPushConstantRange.stageFlags = pushConstantRange.ShaderStage;
            vkPushConstantRange.offset = pushConstantRange.Offset;
            vkPushConstantRange.size = pushConstantRange.Size;
        }

        auto descriptorSetLayouts = vkShader->GetDescriptorSetLayouts();

        vk::PipelineLayoutCreateInfo layoutInfo{};
        layoutInfo.setSetLayouts(descriptorSetLayouts);
        layoutInfo.setPushConstantRanges(vkPushConstantRanges);

        m_layout = vkDevice.createPipelineLayout(layoutInfo);

        vk::GraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.setLayout(m_layout);
        pipelineInfo.setRenderPass(vkFramebuffer->GetRenderPass());

        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyState{};
        inputAssemblyState.setTopology(Utils::ToVulkanTopology(m_desc.Topology));

        vk::PipelineRasterizationStateCreateInfo rasterizationState{};
        rasterizationState.setPolygonMode(m_desc.Wireframe ? vk::PolygonMode::eLine : vk::PolygonMode::eFill);
        rasterizationState.setCullMode(Utils::ToVulkanCullMode(m_desc.CullMode));
        rasterizationState.setFrontFace(vk::FrontFace::eCounterClockwise);
        rasterizationState.setDepthClampEnable(false);
        rasterizationState.setRasterizerDiscardEnable(false);
        rasterizationState.setDepthBiasClamp(false);
        rasterizationState.setDepthBiasEnable(m_desc.DepthBias);
        rasterizationState.setDepthBiasConstantFactor(m_desc.DepthBiasConstantFactor);
        rasterizationState.setDepthBiasSlopeFactor(m_desc.DepthBiasSlopeFactor);
        rasterizationState.setLineWidth(m_desc.LineWidth); // Dynamic

        // Color blend state describes how blend factors are calculated (if used)
        // We need one blend attachment state per color attachment (even if blending is not used)
        auto colorAttachmentCount = desc.Framebuffer->IsSwapChainTarget() ? 1 : desc.Framebuffer->GetColorAttachmentCount();
        std::vector<vk::PipelineColorBlendAttachmentState> blendAttachmentStates(colorAttachmentCount);
        if (m_desc.Framebuffer->IsSwapChainTarget())
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
            for (size_t i = 0; i < colorAttachmentCount; i++)
            {
                blendAttachmentStates[i].setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB |
                    vk::ColorComponentFlagBits::eA);
                blendAttachmentStates[i].setBlendEnable(true);
                blendAttachmentStates[i].setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha);
                blendAttachmentStates[i].setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha);
                blendAttachmentStates[i].setColorBlendOp(vk::BlendOp::eAdd);
                blendAttachmentStates[i].setAlphaBlendOp(vk::BlendOp::eAdd);
                blendAttachmentStates[i].setSrcAlphaBlendFactor(vk::BlendFactor::eOne);
                blendAttachmentStates[i].setDstAlphaBlendFactor(vk::BlendFactor::eZero);
            }
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
            vertexAttributes[location].format = Utils::ToVulkanFormat(element.Type);
            vertexAttributes[location].offset = element.Offset;

            location++;
        }

        vk::PipelineVertexInputStateCreateInfo vertexInputState{};
        vertexInputState.setVertexBindingDescriptions(vertexInputBinding);
        vertexInputState.setVertexAttributeDescriptions(vertexAttributes);

        const auto& shaderStages = vkShader->GetShaderStageCreateInfos();

        pipelineInfo.setStages(shaderStages);
        pipelineInfo.setPVertexInputState(&vertexInputState);
        pipelineInfo.setPInputAssemblyState(&inputAssemblyState);
        pipelineInfo.setPRasterizationState(&rasterizationState);
        pipelineInfo.setPColorBlendState(&colorBlendState);
        pipelineInfo.setPMultisampleState(&multisampleState);
        pipelineInfo.setPViewportState(&viewportState);
        pipelineInfo.setPDepthStencilState(&depthStencilState);
        pipelineInfo.setPDynamicState(&dynamicState);

        m_pipeline = vkDevice.createGraphicsPipeline({}, pipelineInfo).value;
    }

    VulkanPipeline::~VulkanPipeline()
    {
        auto* backend = VulkanBackend::Get();
        auto vkDevice = backend->GetDevice().GetHandle();

        vkDevice.destroy(m_layout);
        vkDevice.destroy(m_pipeline);
    }
}
