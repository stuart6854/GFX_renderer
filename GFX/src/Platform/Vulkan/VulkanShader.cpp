#include "VulkanShader.h"

#include "GFX/Debug.h"
#include "VulkanBackend.h"
#include "VulkanDevice.h"

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_glsl.hpp>

namespace gfx
{
    namespace Utils
    {
        auto VkShaderStageToShaderC(vk::ShaderStageFlagBits stage) -> shaderc_shader_kind
        {
            switch (stage)
            {
                case vk::ShaderStageFlagBits::eVertex: return shaderc_vertex_shader;
                case vk::ShaderStageFlagBits::eFragment: return shaderc_fragment_shader;
                case vk::ShaderStageFlagBits::eCompute: return shaderc_compute_shader;
            }
            return (shaderc_shader_kind)0;
        }
    }

    VulkanShader::VulkanShader(const std::string& vertexSource, const std::string& pixelSource)
    {
        m_shaderSources[vk::ShaderStageFlagBits::eVertex] = vertexSource;
        m_shaderSources[vk::ShaderStageFlagBits::eFragment] = pixelSource;

        auto shaderData = Compile();
        LoadAndCreateShaders(shaderData);
        ReflectAllStages(shaderData);
        CreateDescriptors();
    }

    VulkanShader::~VulkanShader()
    {
        auto* backend = VulkanBackend::Get();
        auto vkDevice = backend->GetDevice().GetHandle();

        for (auto& createInfo : m_pipelineShaderStageCreateInfos)
        {
            vkDevice.destroy(createInfo.module);
        }
    }

    auto VulkanShader::Compile() -> std::unordered_map<vk::ShaderStageFlagBits, std::vector<uint32_t>>
    {
        std::unordered_map<vk::ShaderStageFlagBits, std::vector<uint32_t>> shaderData;

        for (auto& [stage, source] : m_shaderSources)
        {
            shaderc::Compiler compiler;
            shaderc::CompileOptions options;
            options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
            options.SetWarningsAsErrors();
            options.SetGenerateDebugInfo();

            const bool optimize = false;
            if (optimize) options.SetOptimizationLevel(shaderc_optimization_level_performance);

            // Compile Shader
            {
                auto& shaderSource = m_shaderSources.at(stage);
                auto module = compiler.CompileGlslToSpv(shaderSource, Utils::VkShaderStageToShaderC(stage), "", options);
                if (module.GetCompilationStatus() != shaderc_compilation_status_success)
                {
                    GFX_ERROR("{}", module.GetErrorMessage());
                }

                const auto* begin = (const uint8_t*)module.cbegin();
                const auto* end = (const uint8_t*)module.cend();
                const auto size = end - begin;

                shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());
            }
        }

        return shaderData;
    }

    void VulkanShader::LoadAndCreateShaders(const std::unordered_map<vk::ShaderStageFlagBits, std::vector<uint32_t>>& shaderData)
    {
        auto* backend = VulkanBackend::Get();
        auto vkDevice = backend->GetDevice().GetHandle();

        for (auto& [stage, data] : shaderData)
        {
            vk::ShaderModuleCreateInfo moduleInfo{};
            moduleInfo.setCode(data);

            auto module = vkDevice.createShaderModule(moduleInfo);

            auto& shaderStage = m_pipelineShaderStageCreateInfos.emplace_back();
            shaderStage.setStage(stage);
            shaderStage.setModule(module);
            shaderStage.setPName("main");
        }
    }

    void VulkanShader::Reflect(vk::ShaderStageFlagBits stage, const std::vector<uint32_t>& data)
    {
    }

    void VulkanShader::ReflectAllStages(const std::unordered_map<vk::ShaderStageFlagBits, std::vector<uint32_t>>& shaderData)
    {
        // m_resources.clear();

        for (auto& [stage, data] : shaderData)
        {
            Reflect(stage, data);
        }
    }

    void VulkanShader::CreateDescriptors()
    {
    }
}
