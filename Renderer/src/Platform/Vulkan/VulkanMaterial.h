//
// Created by stumi on 21/06/21.
//

#ifndef PERSONAL_RENDERER_VULKANMATERIAL_H
#define PERSONAL_RENDERER_VULKANMATERIAL_H

#include "GFX/Config.h"

#include "VulkanShader.h"
#include "GFX/Resources/RawBuffer.h"
#include "GFX/Utility/Color.h"

#include <vulkan/vulkan.hpp>

#include <glm/glm.hpp>

#include <cstdint>
#include <vector>

namespace gfx
{
    class Material
    {
    public:
        Material(std::shared_ptr<Shader> shader);

        void Set(const std::string& name, float value);
        void Set(const std::string& name, int value);
        void Set(const std::string& name, bool value);
        void Set(const std::string& name, const glm::vec2& value);
        void Set(const std::string& name, const glm::vec3& value);
        void Set(const std::string& name, const glm::vec4& value);
        void Set(const std::string& name, const glm::mat3& value);
        void Set(const std::string& name, const glm::mat4& value);

        auto GetFloat(const std::string& name) -> float&;
        auto GetInt(const std::string& name) -> int&;
        auto GetBool(const std::string& name) -> bool&;
        auto GetVec2(const std::string& name) -> glm::vec2&;
        auto GetVec3(const std::string& name) -> glm::vec3&;
        auto GetVec4(const std::string& name) -> glm::vec4&;
        auto GetMat3(const std::string& name) -> glm::mat3&;
        auto GetMat4(const std::string& name) -> glm::mat4&;

        template <typename T>
        void Set(const std::string& name, const T& value);

        template <typename T>
        auto Get(const std::string& name) -> T&;

        auto GetShader() const -> const std::shared_ptr<Shader>& { return m_shader; }

        auto GetUniformStorageBuffer() const -> const RawBuffer& { return m_uniformStorageBuffer; }

        auto GetDescriptorSet(uint32_t frameIndex) const -> vk::DescriptorSet;

        void UpdateForRendering(uint32_t frameIndex, const std::vector<std::vector<vk::WriteDescriptorSet>>& uniformBufferWriteDescriptors = {});

    private:
        void Init();

        void AllocateStorage();

        auto FindUniformDeclaration(const std::string& name) -> const ShaderUniform*;

    private:
        std::shared_ptr<Shader> m_shader;

        RawBuffer m_uniformStorageBuffer;

        std::array<Shader::ShaderMaterialDescriptorSet, Config::FramesInFlight> m_descriptorSets;

        std::vector<std::vector<vk::WriteDescriptorSet>> m_writeDescriptors;
    };

    template <typename T>
    void Material::Set(const std::string& name, const T& value)
    {
        auto* decl = FindUniformDeclaration(name);
        if (decl == nullptr) return;

        auto& buffer = m_uniformStorageBuffer;
        buffer.Write((uint8_t*)&value, decl->GetSize(), decl->GetOffset());
    }

    template <typename T>
    auto Material::Get(const std::string& name) -> T&
    {
        auto* decl = FindUniformDeclaration(name);
        // TODO: Assert decl
        auto& buffer = m_uniformStorageBuffer;
        return buffer.Read<T>(decl->GetOffset());
    }

}  // namespace gfx

#endif  // PERSONAL_RENDERER_VULKANMATERIAL_H
