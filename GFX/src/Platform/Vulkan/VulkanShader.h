#pragma once

#include "GFX/Resources/Shader.h"

#include <vulkan/vulkan.hpp>

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

namespace gfx
{
    class ResourceSetLayout;

    class VulkanShader : public Shader
    {
    public:
        struct UniformBuffer
        {
            vk::DescriptorBufferInfo Descriptor = {};
            size_t Size = 0;
            uint32_t BindingPoint = 0;
            std::string Name;
            vk::ShaderStageFlagBits ShaderStage = {};
        };

        struct PushConstantRange
        {
            vk::ShaderStageFlagBits ShaderStage = {};
            size_t Offset = 0;
            size_t Size = 0;
        };

        struct ImageSampler
        {
            uint32_t BindingPoint = 0;
            uint32_t DescriptorSet = 0;
            size_t ArraySize = 0;
            std::string Name;
            vk::ShaderStageFlagBits ShaderStage = {};
        };

        struct ShaderDescriptorSet
        {
            std::unordered_map<uint32_t, UniformBuffer> UniformBuffers;
            std::unordered_map<uint32_t, ImageSampler> ImageSamplers;

            std::unordered_map<std::string, vk::WriteDescriptorSet> WriteDescriptorSets;

            operator bool() const { return !(UniformBuffers.empty()); }
        };

        struct ShaderMaterialDescriptorSet
        {
            vk::DescriptorPool Pool = {};
            std::vector<vk::DescriptorSet> DescriptorSets;
        };

    public:
        VulkanShader(const std::string& vertexSource, const std::string& pixelSource);
        ~VulkanShader();

        auto GetShaderBuffers() const -> const std::unordered_map<std::string, ShaderBuffer>& override { return m_buffers; }
        auto GetShaderResources() const -> const std::unordered_map<std::string, ShaderResourceDeclaration>& override { return m_resources; }

        auto GetShaderStageCreateInfos() const -> const std::vector<vk::PipelineShaderStageCreateInfo>& { return m_pipelineShaderStageCreateInfos; }

        auto GetPushConstantRanges() const -> const std::vector<PushConstantRange>& { return m_pushConstantRanges; }
        auto GetDescriptorSetLayouts() const -> std::vector<vk::DescriptorSetLayout>;

    private:
        auto Compile() -> std::unordered_map<vk::ShaderStageFlagBits, std::vector<uint32_t>>;
        void LoadAndCreateShaders(const std::unordered_map<vk::ShaderStageFlagBits, std::vector<uint32_t>>& shaderData);
        void Reflect(vk::ShaderStageFlagBits stage, const std::vector<uint32_t>& data);
        void ReflectAllStages(const std::unordered_map<vk::ShaderStageFlagBits, std::vector<uint32_t>>& shaderData);
        void CreateDescriptors();

    private:
        std::unordered_map<vk::ShaderStageFlagBits, std::string> m_shaderSources;
        std::vector<vk::PipelineShaderStageCreateInfo> m_pipelineShaderStageCreateInfos;

        std::vector<ShaderDescriptorSet> m_shaderDescriptorSets;
        std::unordered_map<std::string, ShaderResourceDeclaration> m_resources;

        std::vector<PushConstantRange> m_pushConstantRanges;
        std::unordered_map<std::string, ShaderBuffer> m_buffers;

        /* Descriptors */
        std::unordered_map<uint32_t, std::vector<vk::DescriptorPoolSize>> m_typeCounts;
        // std::vector<OwnedPtr<ResourceSetLayout>> m_descriptorSetLayouts;
        vk::DescriptorSet m_descriptorSet;
    };
}
