//
// Created by stumi on 12/06/21.
//

#ifdef GFX_API_VULKAN

    #include "VulkanShader.h"

    #include "GFX/Debug.h"
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

    static std::unordered_map<uint32_t, std::unordered_map<uint32_t, Shader::UniformBuffer*>> s_UniformBuffers;  // set -> binding point -> buffer
    //    static std::unordered_map<uint32_t, std::unordered_map<uint32_t, Shader::StorageBuffer*>> s_StorageBuffers;  // set -> binding point -> buffer

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
        CreateDescriptors();
    }

    auto Shader::GetHash() const -> size_t { return std::hash<std::string>{}(m_path); }

    auto Shader::GetAllDescriptorSetLayouts() -> std::vector<vk::DescriptorSetLayout>
    {
        std::vector<vk::DescriptorSetLayout> result;

        result.reserve(m_descriptorSetLayouts.size());
        for (auto& layout : m_descriptorSetLayouts)
        {
            result.emplace_back(layout);
        }

        return result;
    }

    auto Shader::AllocateDescriptorSet(uint32_t set, uint32_t frameIndex) -> Shader::ShaderMaterialDescriptorSet
    {
        GFX_ASSERT(set < m_descriptorSetLayouts.size(), "Cannot allocate unused set!");

        ShaderMaterialDescriptorSet result;
        if (m_shaderDescriptorSets.empty()) return result;

        vk::DescriptorSet descriptorSet = Vulkan::AllocateDescriptorSet(frameIndex, m_descriptorSetLayouts);
        result.DescriptorSets.push_back(descriptorSet);

        return result;
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
            GFX_ERROR("Could not load shader!");
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
        GFX_INFO("===========================");
        GFX_INFO(" Vulkan Shader Reflection");
        GFX_INFO(" {}", m_path);
        GFX_INFO("===========================");

        spirv_cross::Compiler compiler(shaderData);
        auto resources = compiler.get_shader_resources();

        GFX_INFO("Uniform Buffers");
        for (const auto& resource : resources.uniform_buffers)
        {
            const auto& bufferName = resource.name;
            auto& bufferType = compiler.get_type(resource.base_type_id);
            auto bufferSize = compiler.get_declared_struct_size(bufferType);
            auto memberCount = bufferType.member_types.size();
            uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
            uint32_t descriptorSet = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);

            if (descriptorSet >= m_shaderDescriptorSets.size()) m_shaderDescriptorSets.resize(descriptorSet + 1);

            ShaderDescriptorSet& shaderDescriptorSet = m_shaderDescriptorSets[descriptorSet];
            if (s_UniformBuffers[descriptorSet].find(binding) == s_UniformBuffers[descriptorSet].end())
            {
                auto* uniformBuffer = new UniformBuffer;
                uniformBuffer->BindingPoint = binding;
                uniformBuffer->Size = bufferSize;
                uniformBuffer->Name = bufferName;
                uniformBuffer->ShaderStage = shaderStage;
                s_UniformBuffers.at(descriptorSet)[binding] = uniformBuffer;
            }
            else
            {
                auto* uniformBuffer = s_UniformBuffers.at(descriptorSet).at(binding);
                if (bufferSize > uniformBuffer->Size) uniformBuffer->Size = bufferSize;
            }

            shaderDescriptorSet.UniformBuffers[binding] = s_UniformBuffers.at(descriptorSet).at(binding);

            GFX_INFO("  {} ({}, {})", bufferName, descriptorSet, binding);
            GFX_INFO("  Member Count: {}", memberCount);
            GFX_INFO("  Size: {}", bufferSize);
            GFX_INFO("-------------------");
        }

        GFX_INFO("Push Constant Buffers: ");
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

            //  Skip empty push constant buffers - these are for the renderer only
            if (bufferName.empty() || bufferName == "u_Renderer") continue;

            ShaderBuffer& buffer = m_buffers[bufferName];
            buffer.Name = bufferName;
            buffer.Size = bufferSize - bufferOffset;

            GFX_INFO("  Name: {}", bufferName);
            GFX_INFO("  Member Count: {}", memberCount);
            GFX_INFO("  Size: {}", bufferSize);

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

        GFX_INFO("===========================");
    }

    void Shader::ReflectAllStages(const std::unordered_map<vk::ShaderStageFlagBits, std::vector<uint32_t>>& shaderData)
    {
        m_resources.clear();

        for (auto [stage, data] : shaderData)
        {
            Reflect(stage, data);
        }
    }

    void Shader::CreateDescriptors()
    {
        auto device = Vulkan::GetDevice();

        /*
         * Descriptor Pool
         */
        m_typeCounts.clear();
        for (uint32_t set = 0; set < m_shaderDescriptorSets.size(); set++)
        {
            auto& shaderDescriptorSet = m_shaderDescriptorSets[set];

            if (!shaderDescriptorSet.UniformBuffers.empty())
            {
                auto& typeCount = m_typeCounts[set].emplace_back();
                typeCount.setType(vk::DescriptorType::eUniformBuffer);
                typeCount.setDescriptorCount(shaderDescriptorSet.UniformBuffers.size());
            }

            /*
             * Descriptor Set Layout
             */

            std::vector<vk::DescriptorSetLayoutBinding> layoutBindings;
            for (auto& [binding, uniformBuffer] : shaderDescriptorSet.UniformBuffers)
            {
                auto& layoutBinding = layoutBindings.emplace_back();
                layoutBinding.setDescriptorType(vk::DescriptorType::eUniformBuffer);
                layoutBinding.setDescriptorCount(1);
                layoutBinding.setStageFlags(uniformBuffer->ShaderStage);
                layoutBinding.binding = binding;

                auto& writeSet = shaderDescriptorSet.WriteDescriptorSets[uniformBuffer->Name];
                writeSet.setDescriptorType(vk::DescriptorType::eUniformBuffer);
                writeSet.setDescriptorCount(1);
                writeSet.setDstBinding(layoutBinding.binding);
            }

            vk::DescriptorSetLayoutCreateInfo descriptorLayout{};
            descriptorLayout.setBindings(layoutBindings);

            GFX_INFO(
                "Creating descriptor set {} with {} ubo's" /*, {} ssbo's, {} samplers and {} storage images"*/, set, shaderDescriptorSet.UniformBuffers.size());

            if (set >= m_descriptorSetLayouts.size()) m_descriptorSetLayouts.resize(set + 1);
            m_descriptorSetLayouts[set] = device.createDescriptorSetLayout(descriptorLayout);
        }
    }

}  // namespace gfx

#endif