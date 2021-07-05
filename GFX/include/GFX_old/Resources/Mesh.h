//
// Created by stumi on 07/06/21.
//

#ifndef PERSONAL_RENDERER_MESH_H
#define PERSONAL_RENDERER_MESH_H

#include "Vertex.h"
#include "Buffer.h"

#include <glm/mat4x4.hpp>

#include <assimp/scene.h>

#include <string>
#include <vector>

namespace gfxOld
{
    class DeviceContext;
    class Texture;
    class Material;

    class Submesh
    {
    public:
        uint32_t BaseVertex;
        uint32_t BaseIndex;
        uint32_t MaterialIndex;
        uint32_t VertexCount;
        uint32_t IndexCount;
        glm::mat4 Transform = glm::mat4(1.0f);
    };

    class Mesh
    {
    public:
        Mesh(DeviceContext& deviceCtx, const std::string& path);
        Mesh(DeviceContext& deviceCtx, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

        ~Mesh() = default;

        auto GetIndexCount() const -> uint32_t { return m_indices.size(); }

        auto GetVertices() const -> const std::vector<Vertex>& { return m_vertices; }
        auto GetIndices() const -> const std::vector<uint32_t>& { return m_indices; }

        auto GetSubmeshes() const -> const std::vector<Submesh>& { return m_submeshes; }
        auto GetTextures() const -> const std::vector<std::shared_ptr<Texture>>& { return m_textures; }
        auto GetMaterials() const -> const std::vector<std::shared_ptr<Material>>& { return m_materials; }

        auto GetVertexBuffer() -> std::shared_ptr<Buffer> { return m_vertexBuffer; }
        auto GetIndexBuffer() -> std::shared_ptr<Buffer> { return m_indexBuffer; }

    private:
        void LoadMesh(const std::string& path);

        void TraverseNodes(aiNode* node, const glm::mat4& parentTransform = glm::mat4(1.0f), uint32_t level = 0);

    private:
        DeviceContext& m_deviceCtx;

        std::vector<Vertex> m_vertices;
        std::vector<uint32_t> m_indices;

        std::vector<Submesh> m_submeshes;
        std::vector<std::shared_ptr<Texture>> m_textures;
        std::vector<std::shared_ptr<Material>> m_materials;

        std::shared_ptr<Buffer> m_vertexBuffer;
        std::shared_ptr<Buffer> m_indexBuffer;
    };
}  // namespace gfxOld

#endif  // PERSONAL_RENDERER_MESH_H
