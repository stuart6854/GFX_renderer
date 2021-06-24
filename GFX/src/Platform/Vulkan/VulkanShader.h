//
// Created by stumi on 12/06/21.
//

#ifndef PERSONAL_RENDERER_VULKANSHADER_H
#define PERSONAL_RENDERER_VULKANSHADER_H

#include "GFX/Resources/ShaderUniform.h"

#include <vulkan/vulkan.hpp>
#include <shaderc/shaderc.hpp>

#include <string>
#include <vector>
#include <unordered_map>

namespace gfx
{
    class Shader
    {
    public:
        struct UniformBuffer
        {
            vk::DescriptorBufferInfo Descriptor = {};
            uint32_t Size = 0;
            uint32_t BindingPoint = 0;
            std::string Name;
            vk::ShaderStageFlagBits ShaderStage = {};
        };

        struct PushConstantRange
        {
            vk::ShaderStageFlagBits ShaderStage = {};
            uint32_t Offset = 0;
            uint32_t Size = 0;
        };

        struct ImageSampler
        {
            uint32_t BindingPoint = 0;
            uint32_t DescriptorSet = 0;
            uint32_t ArraySize = 0;
            std::string Name;
            vk::ShaderStageFlagBits ShaderStage = {};
        };

        struct ShaderDescriptorSet
        {
            std::unordered_map<uint32_t, UniformBuffer*> UniformBuffers;
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
        Shader(const std::string& path, bool forceCompile = false);

        void Reload(bool forceCompile = false);

        auto GetName() const -> const std::string& { return m_name; }
        auto GetHash() const -> size_t;

        auto GetShaderBuffers() const -> const std::unordered_map<std::string, ShaderBuffer>& { return m_buffers; }
        auto GetShaderResources() const -> const std::unordered_map<std::string, ShaderResourceDeclaration>& { return m_resources; }

        auto GetPipelineShaderStageCreateInfos() const -> const std::vector<vk::PipelineShaderStageCreateInfo>& { return m_pipelineShaderStageCreateInfos; }

        auto GetDescriptorSet() -> vk::DescriptorSet { return m_descriptorSet; }
        auto GetDescriptorSetLayout(uint32_t set) -> vk::DescriptorSetLayout { return m_descriptorSetLayouts.at(set); }
        auto GetAllDescriptorSetLayouts() -> std::vector<vk::DescriptorSetLayout>;

        auto GetShaderDescriptorSets() const -> const std::vector<ShaderDescriptorSet>& { return m_shaderDescriptorSets; }
        auto HasDescriptorSet(uint32_t set) const -> bool { return m_typeCounts.find(set) != m_typeCounts.end(); }

        auto GetPushConstantRanges() const -> const std::vector<PushConstantRange>& { return m_pushConstantRanges; }

        auto AllocateDescriptorSet(uint32_t set = 0, uint32_t frameIndex = 0) -> ShaderMaterialDescriptorSet;
        auto GetDescriptorSet(const std::string& name, uint32_t set = 0) const -> const vk::WriteDescriptorSet*;

    private:
        static auto ReadShaderFromFile(const std::string& filepath) -> std::string;
        static auto ShaderTypeFromString(const std::string& type) -> vk::ShaderStageFlagBits;
        static auto VkShaderStageToShaderC(vk::ShaderStageFlagBits stage) -> shaderc_shader_kind;

        auto PreProcess(const std::string& source) -> std::unordered_map<vk::ShaderStageFlagBits, std::string>;
        auto CompileOrGetVulkanBinary(bool forceCompile) -> std::unordered_map<vk::ShaderStageFlagBits, std::vector<uint32_t>>;
        void LoadAndCreateShaders(std::unordered_map<vk::ShaderStageFlagBits, std::vector<uint32_t>>& shaderData);
        void Reflect(vk::ShaderStageFlagBits shaderStage, const std::vector<uint32_t>& shaderData);
        void ReflectAllStages(const std::unordered_map<vk::ShaderStageFlagBits, std::vector<uint32_t>>& shaderData);
        void CreateDescriptors();

    private:
        std::string m_name;
        std::string m_path;

        std::unordered_map<vk::ShaderStageFlagBits, std::string> m_shaderSource;
        std::vector<vk::PipelineShaderStageCreateInfo> m_pipelineShaderStageCreateInfos;

        std::vector<ShaderDescriptorSet> m_shaderDescriptorSets;
        std::unordered_map<std::string, ShaderResourceDeclaration> m_resources;

        std::vector<PushConstantRange> m_pushConstantRanges;
        std::unordered_map<std::string, ShaderBuffer> m_buffers;

        /* Descriptors */
        std::unordered_map<uint32_t, std::vector<vk::DescriptorPoolSize>> m_typeCounts;
        std::vector<vk::DescriptorSetLayout> m_descriptorSetLayouts;
        vk::DescriptorSet m_descriptorSet;
    };
}  // namespace gfx

#endif  // PERSONAL_RENDERER_VULKANSHADER_H
