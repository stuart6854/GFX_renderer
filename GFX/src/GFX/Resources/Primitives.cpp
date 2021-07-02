// https://www.danielsieger.com/blog/2021/05/03/generating-primitive-shapes.html

#include "GFX/Resources/Primitives.h"

#include "GFX/DeviceContext.h"

namespace gfx
{
    auto Primitives::CreatePlane(DeviceContext& deviceCtx, float size, const uint32_t resolution) -> std::shared_ptr<Mesh>
    {
        const float halfSize = size / 2.0f;

        // Generate vertices
        std::vector<Vertex> vertices;
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
                vertices.push_back(p);
                p.Position.x += increment;
                p.TexCoord.x += 1.0f / resolution;
            }

            p.Position.x = start.Position.x;
            p.Position.z += increment;
            p.TexCoord.x = 0.0f;
            p.TexCoord.y += 1.0f / resolution;
        }

        // Generate faces
        std::vector<uint32_t> indices;
        for (uint32_t z = 0; z < resolution; z++)
        {
            for (uint32_t x = 0; x < resolution; x++)
            {
                auto v0 = z + x * (resolution + 1);
                auto v1 = v0 + 1;
                auto v2 = v0 + (resolution + 1);
                auto v3 = v0 + (resolution + 2);

                indices.push_back(v0);
                indices.push_back(v1);
                indices.push_back(v2);

                indices.push_back(v2);
                indices.push_back(v1);
                indices.push_back(v3);
            }
        }

        auto mesh = std::make_shared<Mesh>(deviceCtx, vertices, indices);

        // TODO: Move uploads to Mesh.cpp
        deviceCtx.Upload(mesh->GetVertexBuffer().get(), mesh->GetVertices().data());
        deviceCtx.Upload(mesh->GetIndexBuffer().get(), mesh->GetIndices().data());

        return mesh;
    }

    auto Primitives::CreateBox(DeviceContext& deviceCtx, const glm::vec3& size) -> std::shared_ptr<Mesh>
    {
        const auto hS = size / 2.0f;

        const std::vector<Vertex> vertices = {
            // Front
            { { -hS.x, hS.y, -hS.z }, { 0, 0, -1 }, { 0, 0 } },
            { { -hS.x, -hS.y, -hS.z }, { 0, 0, -1 }, { 0, 1 } },
            { { hS.x, -hS.y, -hS.z }, { 0, 0, -1 }, { 1, 1 } },
            { { hS.x, hS.y, -hS.z }, { 0, 0, -1 }, { 1, 0 } },
            // Back
            { { hS.x, hS.y, hS.z }, { 0, 0, 1 }, { 0, 0 } },
            { { hS.x, -hS.y, hS.z }, { 0, 0, 1 }, { 0, 1 } },
            { { -hS.x, -hS.y, hS.z }, { 0, 0, 1 }, { 1, 1 } },
            { { -hS.x, hS.y, hS.z }, { 0, 0, 1 }, { 1, 0 } },
            // Left
            { { -hS.x, hS.y, hS.z }, { -1, 0, 0 }, { 0, 0 } },
            { { -hS.x, -hS.y, hS.z }, { -1, 0, 0 }, { 0, 1 } },
            { { -hS.x, -hS.y, -hS.z }, { -1, 0, 0 }, { 1, 1 } },
            { { -hS.x, hS.y, -hS.z }, { -1, 0, 0 }, { 1, 0 } },
            // Right
            { { hS.x, hS.y, -hS.z }, { 1, 0, 0 }, { 0, 0 } },
            { { hS.x, -hS.y, -hS.z }, { 1, 0, 0 }, { 0, 1 } },
            { { hS.x, -hS.y, hS.z }, { 1, 0, 0 }, { 1, 1 } },
            { { hS.x, hS.y, hS.z }, { 1, 0, 0 }, { 1, 0 } },
            // Top
            { { -hS.x, hS.y, hS.z }, { 0, 1, 0 }, { 0, 0 } },
            { { -hS.x, hS.y, -hS.z }, { 0, 1, 0 }, { 0, 1 } },
            { { hS.x, hS.y, -hS.z }, { 0, 1, 0 }, { 1, 1 } },
            { { hS.x, hS.y, hS.z }, { 0, 1, 0 }, { 1, 0 } },
            // Bottom
            { { -hS.x, -hS.y, -hS.z }, { 0, -1, 0 }, { 0, 0 } },
            { { -hS.x, -hS.y, hS.z }, { 0, -1, 0 }, { 0, 1 } },
            { { hS.x, -hS.y, hS.z }, { 0, -1, 0 }, { 1, 1 } },
            { { hS.x, -hS.y, -hS.z }, { 0, -1, 0 }, { 1, 0 } },
        };

        const std::vector<uint32_t> indices = {
            // Front
            0,
            1,
            2,
            2,
            3,
            0,
            // Back
            4,
            5,
            6,
            6,
            7,
            4,
            // Left
            8,
            9,
            10,
            10,
            11,
            8,
            // Right
            12,
            13,
            14,
            14,
            15,
            12,
            // Top
            16,
            17,
            18,
            18,
            19,
            16,
            // Bottom
            20,
            21,
            22,
            22,
            23,
            20,
        };

        auto mesh = std::make_shared<Mesh>(deviceCtx, vertices, indices);

        // TODO: Move uploads to Mesh.cpp
        deviceCtx.Upload(mesh->GetVertexBuffer().get(), mesh->GetVertices().data());
        deviceCtx.Upload(mesh->GetIndexBuffer().get(), mesh->GetIndices().data());

        return mesh;
    }

    // auto Primitives::CreateCone() -> std::shared_ptr<Mesh> { return nullptr; }
    //
    // auto Primitives::CreateCylinder() -> std::shared_ptr<Mesh> { return nullptr; }
    //
    // auto Primitives::CreateTorus() -> std::shared_ptr<Mesh> { return nullptr; }
} // namespace gfx
