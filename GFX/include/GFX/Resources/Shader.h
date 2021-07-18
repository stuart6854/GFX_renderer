#pragma once

#include "GFX/Core/Base.h"

#include <cstdint>
#include <string>
#include <unordered_map>

namespace gfx
{
    class ResourceSetLayout;
    class ResourceSet;

    enum class ShaderUniformType
    {
        eNone = 0,
        eBool,
        eInt,
        eUInt,
        eFloat,
        eVec2,
        eVec3,
        eVec4,
        eMat3,
        eMat4
    };

    class ShaderUniform
    {
    public:
        ShaderUniform() = default;

        ShaderUniform(std::string name, ShaderUniformType type, uint32_t size, uint32_t offset)
            : m_name(std::move(name)),
              m_type(type),
              m_size(size),
              m_offset(offset)
        {
        }

        auto GetName() const -> const std::string& { return m_name; }
        auto GetType() const -> ShaderUniformType { return m_type; }
        auto GetSize() const -> uint32_t { return m_size; }
        auto GetOffset() const -> uint32_t { return m_offset; }

        static auto UniformTypeToString(ShaderUniformType type) -> std::string;

    private:
        std::string m_name;
        ShaderUniformType m_type = ShaderUniformType::eNone;
        uint32_t m_size = 0;
        uint32_t m_offset = 0;
    };

    struct ShaderBuffer
    {
        std::string Name;
        uint32_t Size = 0;
        std::unordered_map<std::string, ShaderUniform> Uniforms;
    };

    enum class ShaderStage
    {
        eNone = 0,
        eVertex,
        ePixel
    };

    class ShaderResourceDeclaration
    {
    public:
        ShaderResourceDeclaration() = default;

        ShaderResourceDeclaration(std::string name, uint32_t resourceRegister, uint32_t count)
            : m_name(std::move(name)),
              m_register(resourceRegister),
              m_count(count)
        {
        }

        auto GetName() const -> const std::string& { return m_name; }
        auto GetRegister() const -> uint32_t { return m_register; }
        auto GetCount() const -> uint32_t { return m_count; }

    private:
        std::string m_name;
        uint32_t m_register = 0;
        uint32_t m_count = 0;
    };

    class Shader
    {
    public:
        static auto Create(const std::string& vertexSource, const std::string& pixelSource) -> OwnedPtr<Shader>;

        virtual ~Shader() = default;

        virtual auto GetShaderBuffers() const -> const std::unordered_map<std::string, ShaderBuffer>& = 0;
        virtual auto GetShaderResources() const -> const std::unordered_map<std::string, ShaderResourceDeclaration>& = 0;

        auto CreateResourceSet(uint32_t set) -> OwnedPtr<ResourceSet>;

    protected:
        std::vector<OwnedPtr<ResourceSetLayout>> m_resourceSetLayouts;
    };
}
