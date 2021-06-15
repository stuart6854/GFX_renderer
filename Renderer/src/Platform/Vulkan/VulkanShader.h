//
// Created by stumi on 12/06/21.
//

#ifndef PERSONAL_RENDERER_VULKANSHADER_H
#define PERSONAL_RENDERER_VULKANSHADER_H

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
        Shader(const std::string& path);

        void Reload();

        auto GetPipelineShaderStageCreateInfos() const -> const std::vector<vk::PipelineShaderStageCreateInfo>& { return m_pipelineShaderStageCreateInfos; }

    private:
        static auto ReadShaderFromFile(const std::string& filepath) -> std::string;
        static auto ShaderTypeFromString(const std::string& type) -> vk::ShaderStageFlagBits;
        static auto VkShaderStageToShaderC(vk::ShaderStageFlagBits stage) -> shaderc_shader_kind;

        auto PreProcess(const std::string& source) -> std::unordered_map<vk::ShaderStageFlagBits, std::string>;
        auto CompileOrGetVulkanBinary() -> std::unordered_map<vk::ShaderStageFlagBits, std::vector<uint32_t>>;
        void LoadAndCreateShaders(std::unordered_map<vk::ShaderStageFlagBits, std::vector<uint32_t>>& shaderData);

    private:
        std::string m_name;
        std::string m_path;

        std::unordered_map<vk::ShaderStageFlagBits, std::string> m_shaderSource;
        std::vector<vk::PipelineShaderStageCreateInfo> m_pipelineShaderStageCreateInfos;
    };
}  // namespace gfx

#endif  // PERSONAL_RENDERER_VULKANSHADER_H
