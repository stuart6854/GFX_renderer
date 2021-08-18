#include "GFX/Resources/MeshBuilder.h"

namespace gfx
{
    auto MeshBuilder::CreatePlane(float size, uint32_t resolution) -> MeshBuilder
    {
        MeshBuilder builder;

        const float halfSize = size / 2.0f;

        // Generate vertices
        const float increment = (size / static_cast<float>(resolution));
        Vertex start{};
        start.Position = { -halfSize, 0, -halfSize };
        start.Normal = { 0.0f, 1.0f, 0.0f };
        start.TexCoord = { 0.0f, 0.0f };

        Vertex p = start;
        for (uint32_t z = 0; z < resolution + 1; z++)
        {
            for (uint32_t x = 0; x < resolution + 1; x++)
            {
                builder.AddVertex(p);
                p.Position.x += increment;
                p.TexCoord.x += 1.0f / resolution;
            }

            p.Position.x = start.Position.x;
            p.Position.z += increment;
            p.TexCoord.x = 0.0f;
            p.TexCoord.y += 1.0f / resolution;
        }

        // Generate faces
        // std::vector<uint32_t> indices;
        for (uint32_t z = 0; z < resolution; z++)
        {
            for (uint32_t x = 0; x < resolution; x++)
            {
                auto v0 = z + x * (resolution + 1);
                auto v1 = v0 + 1;
                auto v2 = v0 + (resolution + 2);
                auto v3 = v0 + (resolution + 1);

                builder.AddTriangle(v0, v1, v2);
                builder.AddTriangle(v2, v3, v0);
            }
        }

        return builder;
    }

    auto MeshBuilder::CreateCube(const glm::vec3& size) -> MeshBuilder
    {
        MeshBuilder builder;

        const auto hS = size / 2.0f;

        // Front
        builder.AddVertex({ { -hS.x, hS.y, -hS.z }, { 0, 0, -1 }, { 0, 0 } });
        builder.AddVertex({ { -hS.x, -hS.y, -hS.z }, { 0, 0, -1 }, { 0, 1 } });
        builder.AddVertex({ { hS.x, -hS.y, -hS.z }, { 0, 0, -1 }, { 1, 1 } });
        builder.AddVertex({ { hS.x, hS.y, -hS.z }, { 0, 0, -1 }, { 1, 0 } });
        // Back
        builder.AddVertex({ { hS.x, hS.y, hS.z }, { 0, 0, 1 }, { 0, 0 } });
        builder.AddVertex({ { hS.x, -hS.y, hS.z }, { 0, 0, 1 }, { 0, 1 } });
        builder.AddVertex({ { -hS.x, -hS.y, hS.z }, { 0, 0, 1 }, { 1, 1 } });
        builder.AddVertex({ { -hS.x, hS.y, hS.z }, { 0, 0, 1 }, { 1, 0 } });
        // Left
        builder.AddVertex({ { -hS.x, hS.y, hS.z }, { -1, 0, 0 }, { 0, 0 } });
        builder.AddVertex({ { -hS.x, -hS.y, hS.z }, { -1, 0, 0 }, { 0, 1 } });
        builder.AddVertex({ { -hS.x, -hS.y, -hS.z }, { -1, 0, 0 }, { 1, 1 } });
        builder.AddVertex({ { -hS.x, hS.y, -hS.z }, { -1, 0, 0 }, { 1, 0 } });
        // Right
        builder.AddVertex({ { hS.x, hS.y, -hS.z }, { 1, 0, 0 }, { 0, 0 } });
        builder.AddVertex({ { hS.x, -hS.y, -hS.z }, { 1, 0, 0 }, { 0, 1 } });
        builder.AddVertex({ { hS.x, -hS.y, hS.z }, { 1, 0, 0 }, { 1, 1 } });
        builder.AddVertex({ { hS.x, hS.y, hS.z }, { 1, 0, 0 }, { 1, 0 } });
        // Top
        builder.AddVertex({ { -hS.x, hS.y, hS.z }, { 0, 1, 0 }, { 0, 0 } });
        builder.AddVertex({ { -hS.x, hS.y, -hS.z }, { 0, 1, 0 }, { 0, 1 } });
        builder.AddVertex({ { hS.x, hS.y, -hS.z }, { 0, 1, 0 }, { 1, 1 } });
        builder.AddVertex({ { hS.x, hS.y, hS.z }, { 0, 1, 0 }, { 1, 0 } });
        // Bottom
        builder.AddVertex({ { -hS.x, -hS.y, -hS.z }, { 0, -1, 0 }, { 0, 0 } });
        builder.AddVertex({ { -hS.x, -hS.y, hS.z }, { 0, -1, 0 }, { 0, 1 } });
        builder.AddVertex({ { hS.x, -hS.y, hS.z }, { 0, -1, 0 }, { 1, 1 } });
        builder.AddVertex({ { hS.x, -hS.y, -hS.z }, { 0, -1, 0 }, { 1, 0 } });

        // Front
        builder.AddTriangle(0, 1, 2);
        builder.AddTriangle(2, 3, 0);
        // Back
        builder.AddTriangle(4, 5, 6);
        builder.AddTriangle(6, 7, 4);
        // Left
        builder.AddTriangle(8, 9, 10);
        builder.AddTriangle(10, 11, 8);
        // Right
        builder.AddTriangle(12, 13, 14);
        builder.AddTriangle(14, 15, 12);
        // Top
        builder.AddTriangle(16, 17, 18);
        builder.AddTriangle(18, 19, 16);
        // Bottom
        builder.AddTriangle(20, 21, 22);
        builder.AddTriangle(22, 23, 20);

        return builder;
    }

    auto MeshBuilder::CreateSphere(float radius) -> MeshBuilder
    {
        return {};
    }

    auto MeshBuilder::AddVertex(const Vertex& position) -> uint32_t
    {
        m_vertices.push_back(position);
        return (uint32_t)m_vertices.size() - 1;
    }

    // void MeshBuilder::AddLine(uint32_t a, uint32_t b)
    // {
    //     m_indices.push_back(a);
    //     m_indices.push_back(b);
    // }

    void MeshBuilder::AddTriangle(const uint32_t a, const uint32_t b, const uint32_t c)
    {
        m_indices.push_back(a);
        m_indices.push_back(b);
        m_indices.push_back(c);
    }
}
