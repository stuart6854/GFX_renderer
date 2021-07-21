#pragma once

#include "Vertex.h"

#include <glm/mat4x4.hpp>

#include <assimp/scene.h>

#include <cstdint>
#include <string>
#include <vector>

namespace gfx
{
    struct SubMesh
    {
        uint32_t BaseVertex = 0;
        uint32_t BaseIndex = 0;
        uint32_t MaterialIndex = 0;
        uint32_t VertexCount = 0;
        uint32_t IndexCount = 0;
        glm::mat4 Transform = glm::mat4(1.0f);
    };

    struct MaterialDef
    {
        std::string Name;
        glm::vec3 AmbientColor = { 0, 0, 0 };
        glm::vec3 DiffuseColor = { 0, 0, 0 };
        glm::vec3 SpecularColor = { 0, 0, 0 };
        float Shininess = 0;

        std::string AmbientTexture;
        std::string DiffuseTexture;
        std::string SpecularTexture;

        std::string NormalMap;
    };

    class MeshImporter
    {
    public:
        MeshImporter(const std::string& filename);

        auto GetFilename() const -> const std::string& { return m_filename; }

        auto GetVertices() const -> const std::vector<Vertex>& { return m_vertices; }
        auto GetIndices() const -> const std::vector<uint32_t>& { return m_indices; }
        auto GetSubMeshes() const -> const std::vector<SubMesh>& { return m_subMeshes; }
        auto GetMaterials() const -> const std::vector<MaterialDef>& { return m_materials; }

    private:
        void LoadSubMeshes(const aiScene* scene);
        void LoadMaterials(const aiScene* scene);

        void LoadMesh(const std::string& filename);

        void TraverseNodes(aiNode* node, const glm::mat4& parentTransform = glm::mat4(1.0f), uint32_t level = 0);

    private:
        std::string m_filename;

        std::vector<Vertex> m_vertices = {};
        std::vector<uint32_t> m_indices = {};

        std::vector<SubMesh> m_subMeshes = {};
        std::vector<MaterialDef> m_materials = {};
    };
}
