//
// Created by stumi on 21/06/21.
//

#ifdef GFX_API_VULKAN

    #include "VulkanMaterial.h"

    #include "VulkanCore.h"
    #include "VulkanTexture.h"

    #include "GFX/Debug.h"

    #include <utility>

namespace gfx
{
    Material::Material(std::shared_ptr<Shader> shader) : m_shader(std::move(shader)), m_writeDescriptors(Config::FramesInFlight) { Init(); }

    void Material::Set(const std::string& name, float value) { Set<float>(name, value); }
    void Material::Set(const std::string& name, int value) { Set<int>(name, value); }
    void Material::Set(const std::string& name, bool value) { Set<bool>(name, value); }
    void Material::Set(const std::string& name, const glm::vec2& value) { Set<glm::vec2>(name, value); }
    void Material::Set(const std::string& name, const glm::vec3& value) { Set<glm::vec3>(name, value); }
    void Material::Set(const std::string& name, const glm::vec4& value) { Set<glm::vec4>(name, value); }
    void Material::Set(const std::string& name, const glm::mat3& value) { Set<glm::mat3>(name, value); }
    void Material::Set(const std::string& name, const glm::mat4& value) { Set<glm::mat4>(name, value); }
    void Material::Set(const std::string& name, const std::shared_ptr<Texture>& texture) { SetVulkanDescriptor(name, texture); }

    auto Material::GetFloat(const std::string& name) -> float& { return Get<float>(name); }
    auto Material::GetInt(const std::string& name) -> int& { return Get<int>(name); }
    auto Material::GetBool(const std::string& name) -> bool& { return Get<bool>(name); }
    auto Material::GetVec2(const std::string& name) -> glm::vec2& { return Get<glm::vec2>(name); }
    auto Material::GetVec3(const std::string& name) -> glm::vec3& { return Get<glm::vec3>(name); }
    auto Material::GetVec4(const std::string& name) -> glm::vec4& { return Get<glm::vec4>(name); }
    auto Material::GetMat3(const std::string& name) -> glm::mat3& { return Get<glm::mat3>(name); }
    auto Material::GetMat4(const std::string& name) -> glm::mat4& { return Get<glm::mat4>(name); }
    auto Material::GetTexture(const std::string& name) -> std::shared_ptr<Texture> { return GetResource<Texture>(name); }

    auto Material::GetDescriptorSet(uint32_t frameIndex) const -> vk::DescriptorSet
    {
        auto& descriptorSets = m_descriptorSets[frameIndex].DescriptorSets;
        if (descriptorSets.empty()) return {};
        return descriptorSets.at(0);
    }

    void Material::UpdateForRendering(uint32_t frameIndex, const std::vector<std::vector<vk::WriteDescriptorSet>>& uniformBufferWriteDescriptors)
    {
        auto device = Vulkan::GetDevice();

        m_writeDescriptors[frameIndex].clear();

        if (!uniformBufferWriteDescriptors.empty())
        {
            for (auto& writeDesc : uniformBufferWriteDescriptors[frameIndex])
            {
                m_writeDescriptors[frameIndex].push_back(writeDesc);
            }
        }

        for (auto&& [binding, pd] : m_residentDescriptors)
        {
            if (pd->Type == PendingDescriptorType::eTexture2D)
            {
                const auto texture = pd->Texture;
                pd->ImageInfo = texture->GetVulkanDescriptorInfo();
                pd->WDS.pImageInfo = &pd->ImageInfo;
            }

            m_writeDescriptors[frameIndex].push_back(pd->WDS);
        }

        auto descriptorSet = m_shader->AllocateDescriptorSet(0, frameIndex);
        m_descriptorSets[frameIndex] = descriptorSet;
        for (auto& writeDesc : m_writeDescriptors[frameIndex])
        {
            writeDesc.setDstSet(descriptorSet.DescriptorSets[0]);
        }

        device.updateDescriptorSets(m_writeDescriptors[frameIndex], {});
    }

    void Material::Init() { AllocateStorage(); }

    void Material::AllocateStorage()
    {
        const auto& shaderBuffers = m_shader->GetShaderBuffers();

        if (!shaderBuffers.empty())
        {
            uint32_t size = 0;
            for (auto& [name, buffer] : shaderBuffers)
            {
                size += buffer.Size;
            }

            m_uniformStorageBuffer.Allocate(size);
            m_uniformStorageBuffer.ZeroInitialise();
        }
    }

    void Material::SetVulkanDescriptor(const std::string& name, const std::shared_ptr<Texture>& texture)
    {
        const auto* resource = FindResourceDeclaration(name);
        GFX_ASSERT(resource);

        const auto binding = resource->GetRegister();
        // Texture is already set
        if (binding < m_textures.size() && m_textures[binding] && texture->GetHash() == m_textures[binding]->GetHash()) return;

        if (binding >= m_textures.size()) m_textures.resize(binding + 1);

        m_textures[binding] = texture;

        const auto* wds = m_shader->GetDescriptorSet(name);
        GFX_ASSERT(wds);

        PendingDescriptor pendingDescriptor{};
        pendingDescriptor.Type = PendingDescriptorType::eTexture2D;
        pendingDescriptor.WDS = *wds;
        pendingDescriptor.Texture = texture;
        m_residentDescriptors[binding] = std::make_shared<PendingDescriptor>(pendingDescriptor);
    }

    auto Material::FindUniformDeclaration(const std::string& name) -> const ShaderUniform*
    {
        const auto& shaderBuffers = m_shader->GetShaderBuffers();
        GFX_ASSERT(shaderBuffers.size() <= 1, "Only ONE material buffer supported!");

        if (!shaderBuffers.empty())
        {
            const auto& buffer = shaderBuffers.begin()->second;
            if (buffer.Uniforms.find(name) == buffer.Uniforms.end()) return nullptr;

            return &buffer.Uniforms.at(name);
        }

        return nullptr;
    }

    auto Material::FindResourceDeclaration(const std::string& name) -> const ShaderResourceDeclaration*
    {
        const auto& resources = m_shader->GetShaderResources();
        for (const auto& [n, resource] : resources)
        {
            if (resource.GetName() == name) return &resource;
        }
        return nullptr;
    }

}  // namespace gfx

#endif