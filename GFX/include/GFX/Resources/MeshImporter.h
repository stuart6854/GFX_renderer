#pragma once

#include <glm/mat4x4.hpp>

#include <assimp/scene.h>

#include <cstdint>
#include <string>
#include <vector>

namespace gfx
{
    struct SubMesh
    {
        uint32_t BaseVertex;
        uint32_t BaseIndex;
        uint32_t MaterialIndex;
        uint32_t VertexCount;
        uint32_t IndexCount;
        glm::mat4 Transform = glm::mat4(1.0f);
    };

    class MeshImporter
    {
    public:
        MeshImporter(const std::string& filename);

        auto GetFilename() const -> const std::string& { return m_filename; }

        auto GetVertices() const -> const std::vector<Vertex>& { return m_vertices; }
        auto GetIndices() const -> const std::vector<uint32_t>& { return m_indices; }
        auto GetSubMeshes() const -> const std::vector<SubMesh>& { return m_subMeshes; }
        auto GetTextures() const -> const std::vector<std::string>& { return m_textures; }
        // auto GetMaterials() const -> const std::vector<SubMesh>& { return m_subMeshes; }

    private:
        void LoadMesh(const std::string& filename);

        void TraverseNodes(aiNode* node, const glm::mat4& parentTransform = glm::mat4(1.0f), uint32_t level = 0);

    private:
        std::string m_filename = "";

        std::vector<Vertex> m_vertices = {};
        std::vector<uint32_t> m_indices = {};

        std::vector<SubMesh> m_subMeshes = {};
        std::vector<std::string> m_textures = {};
        // std::vector<SubMesh> m_materials = {};
    };
}
