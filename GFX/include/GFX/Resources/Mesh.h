//
// Created by stumi on 07/06/21.
//

#ifndef PERSONAL_RENDERER_MESH_H
#define PERSONAL_RENDERER_MESH_H

#include "Vertex.h"
#include "Buffer.h"
#include "Material.h"

#include <string>
#include <vector>

namespace gfx
{
    class Submesh
    {
    public:
        uint32_t BaseVertex;
        uint32_t BaseIndex;
        uint32_t MaterialIndex;
        uint32_t VertexCount;
        uint32_t IndexCount;
    };

    class Mesh
    {
    public:
        Mesh() = default;
        Mesh(const std::string& path);

        ~Mesh() = default;

        auto GetIndexCount() const -> uint32_t { return m_indices.size(); }

        auto GetVertices() const -> const std::vector<Vertex>& { return m_vertices; }
        auto GetIndices() const -> const std::vector<uint32_t>& { return m_indices; }

        auto GetSubmeshes() const -> const std::vector<Submesh>& { return m_submeshes; }
        auto GetMaterials() const -> const std::vector<std::shared_ptr<Material>>& { return m_materials; }

        auto GetVertexBuffer() -> std::shared_ptr<Buffer> { return m_vertexBuffer; }
        auto GetIndexBuffer() -> std::shared_ptr<Buffer> { return m_indexBuffer; }

    private:
        void LoadMesh(const std::string& path);

    private:
        std::vector<Vertex> m_vertices;
        std::vector<uint32_t> m_indices;

        std::vector<Submesh> m_submeshes;
        std::vector<std::shared_ptr<Material>> m_materials;

        std::shared_ptr<Buffer> m_vertexBuffer;
        std::shared_ptr<Buffer> m_indexBuffer;
    };
}  // namespace gfx

#endif  // PERSONAL_RENDERER_MESH_H
