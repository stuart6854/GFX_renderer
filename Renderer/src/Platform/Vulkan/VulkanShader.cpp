//
// Created by stumi on 12/06/21.
//

#ifdef GFX_API_VULKAN

    #include "VulkanShader.h"

    #include "GFX/Resources/ShaderUniform.h"

    #include "VulkanCore.h"

    #include <spirv_cross/spirv_glsl.hpp>

    #include <iostream>
    #include <fstream>
    #include <filesystem>

namespace gfx
{
    static auto SPIRTypeToShaderUniformType(const spirv_cross::SPIRType& type)
    {
        switch (type.basetype)
        {
            case spirv_cross::SPIRType::Boolean: return ShaderUniformType::eBool;
            case spirv_cross::SPIRType::Int: return ShaderUniformType::eInt;
            case spirv_cross::SPIRType::UInt: return ShaderUniformType::eUInt;
            case spirv_cross::SPIRType::Float:
                if (type.vecsize == 1) return ShaderUniformType::eFloat;
                if (type.vecsize == 2) return ShaderUniformType::eVec2;
                if (type.vecsize == 3) return ShaderUniformType::eVec3;
                if (type.vecsize == 4) return ShaderUniformType::eVec4;

                if (type.columns == 3) return ShaderUniformType::eMat3;
                if (type.columns == 4) return ShaderUniformType::eMat4;
                break;
        }
        return ShaderUniformType::eNone;
    }

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
        ReflectAllStages(shaderData);
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
            case vk::ShaderStageFlagBits::eVertex: return shaderc_vertex_shader;
            case vk::ShaderStageFlagBits::eFragment: return shaderc_fragment_shader;
            case vk::ShaderStageFlagBits::eCompute: return shaderc_compute_shader;
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

    void Shader::Reflect(vk::ShaderStageFlagBits shaderStage, const std::vector<uint32_t>& shaderData)
    {
        std::cout << "===========================" << std::endl;
        std::cout << " Vulkan Shader Reflection" << std::endl;
        std::cout << " " << m_path << std::endl;
        std::cout << "===========================" << std::endl;

        spirv_cross::Compiler compiler(shaderData);
        auto resources = compiler.get_shader_resources();

        std::cout << "Push Constant Buffers: " << std::endl;
        for (const auto& resource : resources.push_constant_buffers)
        {
            const auto& bufferName = resource.name;
            auto& bufferType = compiler.get_type(resource.base_type_id);
            auto bufferSize = compiler.get_declared_struct_size(bufferType);
            auto memberCount = bufferType.member_types.size();
            uint32_t bufferOffset = 0;
            if (!m_pushConstantRanges.empty())
            {
                bufferOffset = m_pushConstantRanges.back().Offset + m_pushConstantRanges.back().Size;
            }

            auto& pushConstantRange = m_pushConstantRanges.emplace_back();
            pushConstantRange.ShaderStage = shaderStage;
            pushConstantRange.Offset = bufferOffset;
            pushConstantRange.Size = bufferSize;

            // TODO: ??
            //  Skip empty push constant buffers - these are for the renderer only
            if (bufferName.empty() || bufferName == "u_Renderer") continue;

            ShaderBuffer& buffer = m_buffers[bufferName];
            buffer.Name = bufferName;
            buffer.Size = bufferSize;

            std::cout << "  Name: " << bufferName << std::endl;
            std::cout << "  Member Count: " << memberCount << std::endl;
            std::cout << "  Size: " << bufferSize << std::endl;

            for (int i = 0; i < memberCount; i++)
            {
                auto& type = compiler.get_type(bufferType.member_types[i]);
                const auto& memberName = compiler.get_member_name(bufferType.self, i);
                auto size = compiler.get_declared_struct_member_size(bufferType, i);
                auto offset = compiler.type_struct_member_offset(bufferType, i) - bufferOffset;

                std::string uniformName = bufferName + "." + memberName;
                buffer.Uniforms[uniformName] = ShaderUniform(uniformName, SPIRTypeToShaderUniformType(type), size, offset);
            }
        }

        std::cout << "===========================" << std::endl;
    }

    void Shader::ReflectAllStages(const std::unordered_map<vk::ShaderStageFlagBits, std::vector<uint32_t>>& shaderData)
    {
        m_resources.clear();

        for (auto [stage, data] : shaderData)
        {
            Reflect(stage, data);
        }
    }

}  // namespace gfx

#endif