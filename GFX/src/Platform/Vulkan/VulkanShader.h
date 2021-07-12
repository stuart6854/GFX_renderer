#pragma once

#include "GFX/Resources/Shader.h"

#include <vulkan/vulkan.hpp>

#include <cstdint>
#include <vector>
#include <unordered_map>

namespace gfx
{
    class VulkanShader : public Shader
    {
    public:
        VulkanShader(const std::string& vertexSource, const std::string& pixelSource);
        ~VulkanShader();

        auto GetShaderStageCreateInfos() const -> const std::vector<vk::PipelineShaderStageCreateInfo>& { return m_pipelineShaderStageCreateInfos; }

    private:
        auto Compile() -> std::unordered_map<vk::ShaderStageFlagBits, std::vector<uint32_t>>;
        void LoadAndCreateShaders(const std::unordered_map<vk::ShaderStageFlagBits, std::vector<uint32_t>>& shaderData);
        void Reflect(vk::ShaderStageFlagBits stage, const std::vector<uint32_t>& data);
        void ReflectAllStages(const std::unordered_map<vk::ShaderStageFlagBits, std::vector<uint32_t>>& shaderData);
        void CreateDescriptors();

    private:
        std::unordered_map<vk::ShaderStageFlagBits, std::string> m_shaderSources;
        std::vector<vk::PipelineShaderStageCreateInfo> m_pipelineShaderStageCreateInfos;
    };
}
