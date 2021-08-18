#include "VulkanShader.h"

#include "GFX/Debug.h"
#include "GFX/Resources/ResourceSetLayout.h"
#include "VulkanBackend.h"
#include "VulkanDevice.h"
#include "VulkanResourceSetLayout.h"
#include "VulkanUtils.h"

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

        auto ToShaderUniformType(const spirv_cross::SPIRType& type)
        {
            switch (type.basetype)
            {
                case spirv_cross::SPIRType::Boolean: return ShaderUniformType::eBool;
                case spirv_cross::SPIRType::Int: return ShaderUniformType::eInt;
                case spirv_cross::SPIRType::UInt: return ShaderUniformType::eUInt;
                case spirv_cross::SPIRType::Float: if (type.vecsize == 1) return ShaderUniformType::eFloat;
                    if (type.vecsize == 2) return ShaderUniformType::eVec2;
                    if (type.vecsize == 3) return ShaderUniformType::eVec3;
                    if (type.vecsize == 4) return ShaderUniformType::eVec4;

                    if (type.columns == 3) return ShaderUniformType::eMat3;
                    if (type.columns == 4) return ShaderUniformType::eMat4;
                    break;
            }
            return ShaderUniformType::eNone;
        }
    }

    static std::unordered_map<uint32_t, std::unordered_map<uint32_t, VulkanShader::UniformBuffer>> s_UniformBuffers; // set -> binding point -> buffer
    //    static std::unordered_map<uint32_t, std::unordered_map<uint32_t, VulkanShader::StorageBuffer*>> s_StorageBuffers;  // set -> binding point -> buffer

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

    auto VulkanShader::GetDescriptorSetLayouts() const -> std::vector<vk::DescriptorSetLayout>
    {
        std::vector<vk::DescriptorSetLayout> layouts;

        for (auto& set : m_resourceSetLayouts)
        {
            auto* vkSet = static_cast<VulkanResourceSetLayout*>(set.get());
            layouts.emplace_back(vkSet->GetHandle());
        }

        return layouts;
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
                    GFX_ERROR("Shader Error | Stage = {} \n{}", vk::to_string(stage), module.GetErrorMessage());
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
        GFX_TRACE("===========================");
        GFX_TRACE(" Vulkan Shader Reflection");
        GFX_TRACE(" Stage: {}", vk::to_string(stage));
        GFX_TRACE("===========================");

        spirv_cross::Compiler compiler(data);
        auto resources = compiler.get_shader_resources();

        GFX_TRACE("Uniform Buffers");
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
                auto& uniformBuffer = s_UniformBuffers.at(descriptorSet)[binding];
                uniformBuffer.BindingPoint = binding;
                uniformBuffer.Size = bufferSize;
                uniformBuffer.Name = bufferName;
                uniformBuffer.ShaderStage = stage;
            }
            else
            {
                auto& uniformBuffer = s_UniformBuffers.at(descriptorSet).at(binding);
                if (bufferSize > uniformBuffer.Size) uniformBuffer.Size = bufferSize;
            }

            shaderDescriptorSet.UniformBuffers[binding] = s_UniformBuffers.at(descriptorSet).at(binding);

            GFX_TRACE("  {} ({}, {})", bufferName, descriptorSet, binding);
            GFX_TRACE("    Member Count: {}", memberCount);
            GFX_TRACE("    Size: {}", bufferSize);
        }

        GFX_TRACE("Push Constant Buffers: ");
        for (const auto& resource : resources.push_constant_buffers)
        {
            const auto& bufferName = resource.name;
            auto& bufferType = compiler.get_type(resource.base_type_id);
            auto bufferSize = compiler.get_declared_struct_size(bufferType);
            auto memberCount = bufferType.member_types.size();
            size_t bufferOffset = 0;
            if (!m_pushConstantRanges.empty())
            {
                bufferOffset = m_pushConstantRanges.back().Offset + m_pushConstantRanges.back().Size;
            }

            auto& pushConstantRange = m_pushConstantRanges.emplace_back();
            pushConstantRange.ShaderStage = stage;
            pushConstantRange.Offset = bufferOffset;
            pushConstantRange.Size = bufferSize;

            //  Skip empty push constant buffers - these are for the renderer only
            if (bufferName.empty() || bufferName == "u_Renderer") continue;

            ShaderBuffer& buffer = m_buffers[bufferName];
            buffer.Name = bufferName;
            buffer.Size = bufferSize - bufferOffset;

            GFX_TRACE("  Name: {}", bufferName);
            GFX_TRACE("    Member Count: {}", memberCount);
            GFX_TRACE("    Size: {}", bufferSize);

            for (int i = 0; i < memberCount; i++)
            {
                auto& type = compiler.get_type(bufferType.member_types[i]);
                const auto& memberName = compiler.get_member_name(bufferType.self, i);
                auto size = compiler.get_declared_struct_member_size(bufferType, i);
                auto offset = compiler.type_struct_member_offset(bufferType, i) - bufferOffset;

                std::string uniformName = bufferName + "." + memberName;
                buffer.Uniforms[uniformName] = ShaderUniform(uniformName, Utils::ToShaderUniformType(type), size, offset);
            }
        }

        GFX_TRACE("Sampled Images: ");
        for (const auto& resource : resources.sampled_images)
        {
            const auto& name = resource.name;
//            auto& baseType = compiler.get_type(resource.base_type_id);
            auto& type = compiler.get_type(resource.type_id);
            uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
            uint32_t descriptorSet = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
//            uint32_t dimension = baseType.image.dim;
            uint32_t arraySize = type.array[0];
            if (arraySize == 0) arraySize = 1;
            if (descriptorSet >= m_shaderDescriptorSets.size()) m_shaderDescriptorSets.resize(descriptorSet + 1);

            auto& shaderDescriptorSet = m_shaderDescriptorSets[descriptorSet];
            auto& imageSampler = shaderDescriptorSet.ImageSamplers[binding];
            imageSampler.BindingPoint = binding;
            imageSampler.DescriptorSet = descriptorSet;
            imageSampler.Name = name;
            imageSampler.ArraySize = arraySize;
            imageSampler.ShaderStage = stage;

            m_resources[name] = ShaderResourceDeclaration(name, binding, 1);

            GFX_TRACE("  {}[{}] (set={}, binding={})", name, arraySize,descriptorSet, binding);
        }

        GFX_TRACE("===========================");
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
        auto* backend = VulkanBackend::Get();
        auto vkDevice = backend->GetDevice().GetHandle();

        m_typeCounts.clear();
        m_resourceSetLayouts.resize(m_shaderDescriptorSets.size());
        for (uint32_t set = 0; set < m_shaderDescriptorSets.size(); set++)
        {
            auto& shaderSet = m_shaderDescriptorSets[set];
            auto& layout = m_resourceSetLayouts[set] = ResourceSetLayout::Create();

            if (!shaderSet.UniformBuffers.empty())
            {
                auto& typeCount = m_typeCounts[set].emplace_back();
                typeCount.setType(vk::DescriptorType::eUniformBuffer);
                typeCount.setDescriptorCount((uint32_t)shaderSet.UniformBuffers.size());
            }
            if (!shaderSet.ImageSamplers.empty())
            {
                auto& typeCount = m_typeCounts[set].emplace_back();
                typeCount.setType(vk::DescriptorType::eCombinedImageSampler);
                typeCount.setDescriptorCount((uint32_t)shaderSet.ImageSamplers.size());
            }

            /* Descriptor Set Layout */
            std::vector<vk::DescriptorSetLayoutBinding> layoutBindings;
            for (auto& [binding, uniformBuffer] : shaderSet.UniformBuffers)
            {
                auto& layoutBinding = layoutBindings.emplace_back();
                layoutBinding.setDescriptorType(vk::DescriptorType::eUniformBuffer);
                layoutBinding.setDescriptorCount(1);
                layoutBinding.setStageFlags(uniformBuffer.ShaderStage);
                layoutBinding.setBinding(binding);

                layout->AddBinding(binding, ResourceType::eUniformBuffer, 1, VkUtils::ToShaderStage(uniformBuffer.ShaderStage));

                auto& writeSet = shaderSet.WriteDescriptorSets[uniformBuffer.Name];
                writeSet.setDescriptorType(vk::DescriptorType::eUniformBuffer);
                writeSet.setDescriptorCount(1);
                writeSet.setDstBinding(layoutBinding.binding);
            }
            for (auto& [binding, imageSampler] : shaderSet.ImageSamplers)
            {
                auto& layoutBinding = layoutBindings.emplace_back();
                layoutBinding.setDescriptorType(vk::DescriptorType::eCombinedImageSampler);
                layoutBinding.setDescriptorCount((uint32_t)imageSampler.ArraySize);
                layoutBinding.setStageFlags(imageSampler.ShaderStage);
                layoutBinding.setBinding(binding);

                layout->AddBinding(binding, ResourceType::eTextureSampler, imageSampler.ArraySize, VkUtils::ToShaderStage(imageSampler.ShaderStage));

                GFX_ASSERT(shaderSet.UniformBuffers.find(binding) == shaderSet.UniformBuffers.end(), "Binding is already in use!");

                auto& writeSet = shaderSet.WriteDescriptorSets[imageSampler.Name];
                writeSet.setDescriptorType(vk::DescriptorType::eCombinedImageSampler);
                writeSet.setDescriptorCount((uint32_t)imageSampler.ArraySize);
                writeSet.setDstBinding(layoutBinding.binding);
            }

            layout->Build();

            GFX_INFO("Creating descriptor set {} with {} ubo's and {} samplers",
                     set,
                     shaderSet.UniformBuffers.size(),
                     shaderSet.ImageSamplers.size());
        }
    }
}
