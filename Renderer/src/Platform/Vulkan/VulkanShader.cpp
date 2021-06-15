//
// Created by stumi on 12/06/21.
//

#ifdef GFX_API_VULKAN

    #include "VulkanShader.h"

    #include "VulkanCore.h"

    #include <iostream>
    #include <fstream>
    #include <filesystem>

namespace gfx
{
    Shader::Shader(const std::string& path) : m_path(path)
    {
        // Get name of shader from filename without ext
        auto found = path.find_last_of("/\\");
        m_name = (found != std::string::npos) ? path.substr(found + 1) : path;
        found = m_name.find_last_of('.');
        m_name = (found != std::string::npos) ? path.substr(0, found) : m_name;

        Reload();
    }

    void Shader::Reload()
    {
        auto source = ReadShaderFromFile(m_path);

        m_shaderSource = PreProcess(source);
        auto shaderData = CompileOrGetVulkanBinary();
        LoadAndCreateShaders(shaderData);
    }

    auto Shader::ReadShaderFromFile(const std::string& filepath) -> std::string
    {
        std::string result;
        std::ifstream in(filepath, std::ios::in | std::ios::binary);
        if (in)
        {
            in.seekg(0, std::ios::end);
            result.resize(in.tellg());
            in.seekg(0, std::ios::beg);
            in.read(&result[0], result.size());
        }
        else
        {
            std::cerr << "Could not load shader!" << std::endl;
        }
        in.close();
        return result;
    }

    auto Shader::ShaderTypeFromString(const std::string& type) -> vk::ShaderStageFlagBits
    {
        if (type == "vertex") return vk::ShaderStageFlagBits::eVertex;
        if (type == "fragment" || type == "pixel") return vk::ShaderStageFlagBits::eFragment;
        if (type == "compute") return vk::ShaderStageFlagBits::eCompute;
        return {};
    }

    auto Shader::VkShaderStageToShaderC(vk::ShaderStageFlagBits stage) -> shaderc_shader_kind
    {
        switch (stage)
        {
            case vk::ShaderStageFlagBits::eVertex:
                return shaderc_vertex_shader;
            case vk::ShaderStageFlagBits::eFragment:
                return shaderc_fragment_shader;
            case vk::ShaderStageFlagBits::eCompute:
                return shaderc_compute_shader;
        }
        return (shaderc_shader_kind)0;
    }

    auto Shader::PreProcess(const std::string& source) -> std::unordered_map<vk::ShaderStageFlagBits, std::string>
    {
        std::unordered_map<vk::ShaderStageFlagBits, std::string> shaderSources;

        const char* typeToken = "#type";
        auto typeTokenLength = strlen(typeToken);
        auto pos = source.find(typeToken, 0);
        while (pos != std::string::npos)
        {
            auto eol = source.find_first_of("\r\n", pos);
            auto begin = pos + typeTokenLength + 1;
            auto type = source.substr(begin, eol - begin);

            auto nextLinePos = source.find_first_not_of("\r\n", eol);
            pos = source.find(typeToken, nextLinePos);
            auto shaderType = ShaderTypeFromString(type);
            shaderSources[shaderType] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
        }

        return shaderSources;
    }

    auto Shader::CompileOrGetVulkanBinary() -> std::unordered_map<vk::ShaderStageFlagBits, std::vector<uint32_t>>
    {
        std::unordered_map<vk::ShaderStageFlagBits, std::vector<uint32_t>> shaderData;

        for (auto [stage, source] : m_shaderSource)
        {
            shaderc::Compiler compiler;
            shaderc::CompileOptions options;
            options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
            options.SetWarningsAsErrors();
            options.SetGenerateDebugInfo();

            const bool optimise = false;
            if (optimise) options.SetOptimizationLevel(shaderc_optimization_level_performance);

            // Compile shader
            {
                auto& shaderSource = m_shaderSource.at(stage);
                shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(shaderSource, VkShaderStageToShaderC(stage), m_path.c_str(), options);

                if (module.GetCompilationStatus() != shaderc_compilation_status_success)
                {
                    std::cerr << module.GetErrorMessage() << std::endl;
                }

                const auto* begin = (const uint8_t*)module.cbegin();
                const auto* end = (const uint8_t*)module.cend();
                const auto size = end - begin;

                shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());
            }
        }

        return shaderData;
    }

    void Shader::LoadAndCreateShaders(std::unordered_map<vk::ShaderStageFlagBits, std::vector<uint32_t>>& shaderData)
    {
        auto device = Vulkan::GetDevice();

        for (auto [stage, data] : shaderData)
        {
            vk::ShaderModuleCreateInfo moduleInfo{};
            moduleInfo.setCode(data);

            auto shaderModule = device.createShaderModule(moduleInfo);

            auto& shaderStage = m_pipelineShaderStageCreateInfos.emplace_back();
            shaderStage.setStage(stage);
            shaderStage.setModule(shaderModule);
            shaderStage.setPName("main");
        }
    }

}  // namespace gfx

#endif