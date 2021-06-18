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

        struct ShaderDescriptorSet
        {
            std::unordered_map<uint32_t, UniformBuffer*> UniformBuffers;

            std::unordered_map<std::string, vk::WriteDescriptorSet> WriteDescriptorSets;

            operator bool() const { return !(UniformBuffers.empty()); }
        };

    public:
        Shader(const std::string& path);

        void Reload();

        auto GetPipelineShaderStageCreateInfos() const -> const std::vector<vk::PipelineShaderStageCreateInfo>& { return m_pipelineShaderStageCreateInfos; }

        auto GetShaderDescriptorSets() const -> const std::vector<ShaderDescriptorSet>& { return m_shaderDescriptorSets; }
        auto GetPushConstantRanges() const -> const std::vector<PushConstantRange>& { return m_pushConstantRanges; }

    private:
        static auto ReadShaderFromFile(const std::string& filepath) -> std::string;
        static auto ShaderTypeFromString(const std::string& type) -> vk::ShaderStageFlagBits;
        static auto VkShaderStageToShaderC(vk::ShaderStageFlagBits stage) -> shaderc_shader_kind;

        auto PreProcess(const std::string& source) -> std::unordered_map<vk::ShaderStageFlagBits, std::string>;
        auto CompileOrGetVulkanBinary() -> std::unordered_map<vk::ShaderStageFlagBits, std::vector<uint32_t>>;
        void LoadAndCreateShaders(std::unordered_map<vk::ShaderStageFlagBits, std::vector<uint32_t>>& shaderData);
        void Reflect(vk::ShaderStageFlagBits shaderStage, const std::vector<uint32_t>& shaderData);
        void ReflectAllStages(const std::unordered_map<vk::ShaderStageFlagBits, std::vector<uint32_t>>& shaderData);

    private:
        std::string m_name;
        std::string m_path;

        std::unordered_map<vk::ShaderStageFlagBits, std::string> m_shaderSource;
        std::vector<vk::PipelineShaderStageCreateInfo> m_pipelineShaderStageCreateInfos;

        std::vector<ShaderDescriptorSet> m_shaderDescriptorSets;
        std::unordered_map<std::string, ShaderResourceDeclaration> m_resources;

        std::vector<PushConstantRange> m_pushConstantRanges;
        std::unordered_map<std::string, ShaderBuffer> m_buffers;
    };
}  // namespace gfx

#endif  // PERSONAL_RENDERER_VULKANSHADER_H
