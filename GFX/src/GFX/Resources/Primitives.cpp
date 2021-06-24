﻿// https://www.danielsieger.com/blog/2021/05/03/generating-primitive-shapes.html

#include "GFX/Resources/Primitives.h"

namespace gfx
{
    auto Primitives::CreatePlane(DeviceContext& deviceCtx,
                                 float size,
                                 const uint32_t resolution) -> std::shared_ptr<Mesh>
    {
        // Generate vertices
        std::vector<Vertex> vertices;
        const float increment = (1.0f / static_cast<float>(resolution)) * size;
        Vertex p{};
        p.Position = { 0, 0, 0 };
        p.Normal = { 0, 1, 0 };
        for (uint32_t z = 0; z < resolution + 1; z++)
        {
            for (uint32_t x = 0; x < resolution + 1; x++)
            {
                vertices.push_back(p);
                p.Position.x += increment;
            }

            p.Position.x = 0;
            p.Position.z += increment;
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
                indices.push_back(v2);
                indices.push_back(v1);

                indices.push_back(v2);
                indices.push_back(v3);
                indices.push_back(v1);
            }
        }

        auto mesh = std::make_shared<Mesh>(vertices, indices);

        deviceCtx.Upload(mesh->GetVertexBuffer().get(), mesh->GetVertices().data());
        deviceCtx.Upload(mesh->GetIndexBuffer().get(), mesh->GetIndices().data());

        return mesh;
    }

    // auto Primitives::CreateCone() -> std::shared_ptr<Mesh> { return nullptr; }
    //
    // auto Primitives::CreateCylinder() -> std::shared_ptr<Mesh> { return nullptr; }
    //
    // auto Primitives::CreateTorus() -> std::shared_ptr<Mesh> { return nullptr; }
}  // namespace gfx