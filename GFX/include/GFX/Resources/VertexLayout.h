//
// Created by stumi on 15/06/21.
//

#ifndef PERSONAL_RENDERER_VERTEXLAYOUT_H
#define PERSONAL_RENDERER_VERTEXLAYOUT_H

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace gfx
{
    enum class ShaderDataType
    {
        None = 0,
        Float,
        Float2,
        Float3,
        Float4,
        Mat3,
        Mat4,
        Int,
        Int2,
        Int3,
        Int4,
        Bool
    };

    static auto ShaderDataTypeSize(ShaderDataType type) -> uint32_t
    {
        switch (type)
        {
            case ShaderDataType::None: break;
            case ShaderDataType::Float: return 4;
            case ShaderDataType::Float2: return 4 * 2;
            case ShaderDataType::Float3: return 4 * 3;
            case ShaderDataType::Float4: return 4 * 4;
            case ShaderDataType::Mat3: return 4 * 3 * 3;
            case ShaderDataType::Mat4: return 4 * 4 * 4;
            case ShaderDataType::Int: return 4;
            case ShaderDataType::Int2: return 4 * 2;
            case ShaderDataType::Int3: return 4 * 3;
            case ShaderDataType::Int4: return 4 * 4;
            case ShaderDataType::Bool: return 4;
        }
        return 0;
    }

    struct VertexElement
    {
        std::string Name;
        ShaderDataType Type;
        uint32_t Size;
        uint32_t Offset;
        bool Normalized;

        VertexElement() = default;
        VertexElement(ShaderDataType type, std::string name, bool normalized = false)
            : Name(std::move(name)), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
        {
        }

        auto GetComponentCount() const -> uint32_t
        {
            switch (Type)
            {
                case ShaderDataType::None: break;
                case ShaderDataType::Float: return 1;
                case ShaderDataType::Float2: return 2;
                case ShaderDataType::Float3: return 3;
                case ShaderDataType::Float4: return 4;
                case ShaderDataType::Mat3: return 3 * 3;
                case ShaderDataType::Mat4: return 4 * 4;
                case ShaderDataType::Int: return 1;
                case ShaderDataType::Int2: return 2;
                case ShaderDataType::Int3: return 3;
                case ShaderDataType::Int4: return 4;
                case ShaderDataType::Bool: return 1;
            }
            return 0;
        }
    };

    class VertexLayout
    {
    public:
        VertexLayout() = default;
        VertexLayout(std::initializer_list<VertexElement> elements) : m_elements(elements) { CalculateOffsetAndStride(); }

        auto GetStride() const -> uint32_t { return m_stride; }
        auto GetElements() const -> const std::vector<VertexElement>& { return m_elements; }
        auto GetElementCount() const -> uint32_t { return m_elements.size(); }

        auto begin() -> std::vector<VertexElement>::iterator { return m_elements.begin(); }
        auto end() -> std::vector<VertexElement>::iterator { return m_elements.end(); }
        auto begin() const -> std::vector<VertexElement>::const_iterator { return m_elements.begin(); }
        auto end() const -> std::vector<VertexElement>::const_iterator { return m_elements.end(); }

    private:
        void CalculateOffsetAndStride()
        {
            uint32_t offset = 0;
            m_stride = 0;
            for (auto& element : m_elements)
            {
                element.Offset = offset;
                offset += element.Size;
                m_stride += element.Size;
            }
        }

    private:
        std::vector<VertexElement> m_elements;
        uint32_t m_stride{};
    };
}  // namespace gfx

#endif  // PERSONAL_RENDERER_VERTEXLAYOUT_H
