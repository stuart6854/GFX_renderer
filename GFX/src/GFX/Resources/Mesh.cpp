//
// Created by stumi on 19/06/21.
//

#include "GFX/Resources/Mesh.h"

#include "GFX/Debug.h"
#include "GFX/Resources/Shader.h"
#include "GFX/Resources/ResourceDescriptions.h"
#include "GFX/Utility/Color.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

namespace gfx
{
    static const uint32_t s_meshImportFlags = aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_MakeLeftHanded;

    auto Mat4FromAssimpMat4(const aiMatrix4x4& matrix) -> glm::mat4
    {
        glm::mat4 result;
        // the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
        result[0][0] = matrix.a1;
        result[1][0] = matrix.a2;
        result[2][0] = matrix.a3;
        result[3][0] = matrix.a4;
        result[0][1] = matrix.b1;
        result[1][1] = matrix.b2;
        result[2][1] = matrix.b3;
        result[3][1] = matrix.b4;
        result[0][2] = matrix.c1;
        result[1][2] = matrix.c2;
        result[2][2] = matrix.c3;
        result[3][2] = matrix.c4;
        result[0][3] = matrix.d1;
        result[1][3] = matrix.d2;
        result[2][3] = matrix.d3;
        result[3][3] = matrix.d4;
        return result;
    }

    Mesh::Mesh(const std::string& path) { LoadMesh(path); }

    Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) : m_vertices(vertices), m_indices(indices)
    {
        auto& submesh = m_submeshes.emplace_back();
        submesh.BaseVertex = 0;
        submesh.BaseIndex = 0;
        submesh.VertexCount = vertices.size();
        submesh.IndexCount = indices.size();

        BufferDesc vertexBufferDesc{};
        vertexBufferDesc.Type = BufferType::eVertex;
        vertexBufferDesc.Size = sizeof(Vertex) * submesh.VertexCount;
        m_vertexBuffer = std::make_shared<Buffer>(vertexBufferDesc);

        BufferDesc indexBufferDesc{};
        indexBufferDesc.Type = BufferType::eIndex;
        indexBufferDesc.Size = sizeof(uint32_t) * submesh.IndexCount;
        m_indexBuffer = std::make_shared<Buffer>(indexBufferDesc);

        const auto meshShader = ShaderLibrary::Get("PBR_Static");
        const auto material = std::make_shared<Material>(meshShader);
        material->Set("u_MaterialUniforms.Ambient", { 0.8f, 0.8f, 0.8f });
        material->Set("u_MaterialUniforms.Diffuse", { 0.8f, 0.8f, 0.8f });
        material->Set("u_MaterialUniforms.Specular", { 0.8f, 0.8f, 0.8f });
        m_materials.push_back(material);
    }

    void Mesh::LoadMesh(const std::string& path)
    {
        Assimp::Importer importer;
        auto* scene = importer.ReadFile(path, s_meshImportFlags);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            GFX_ERROR("Assimp Error: {}", importer.GetErrorString());
            return;
        }

        GFX_INFO("Mesh loading: {} meshes, {} materials", scene->mNumMeshes, scene->mNumMaterials);

        uint32_t vertexCount = 0;
        uint32_t indexCount = 0;

        m_submeshes.reserve(scene->mNumMeshes);
        for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++)
        {
            auto* mesh = scene->mMeshes[meshIndex];

            auto& submesh = m_submeshes.emplace_back();
            submesh.BaseVertex = vertexCount;
            submesh.BaseIndex = indexCount;
            submesh.MaterialIndex = mesh->mMaterialIndex;
            submesh.VertexCount = mesh->mNumVertices;
            submesh.IndexCount = mesh->mNumFaces * 3;

            GFX_INFO("  Submesh: {} vertices, {} indices", submesh.VertexCount, submesh.IndexCount);

            vertexCount += mesh->mNumVertices;
            indexCount += submesh.IndexCount;

            GFX_ASSERT(mesh->HasPositions(), "Mesh must have positions!");
            GFX_ASSERT(mesh->HasNormals(), "Mesh must have normals!");

            // Vertices
            for (uint32_t vertIndex = 0; vertIndex < mesh->mNumVertices; vertIndex++)
            {
                auto& vertex = m_vertices.emplace_back();
                vertex.Position = { mesh->mVertices[vertIndex].x, mesh->mVertices[vertIndex].y, mesh->mVertices[vertIndex].z };
                vertex.Normal = { mesh->mNormals[vertIndex].x, mesh->mNormals[vertIndex].y, mesh->mNormals[vertIndex].z };

                if (mesh->HasTextureCoords(0))
                {
                    vertex.TexCoord = { mesh->mTextureCoords[0][vertIndex].x, mesh->mTextureCoords[0][vertIndex].y };
                }
            }

            // Indices
            for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++)
            {
                GFX_ASSERT(mesh->mFaces[faceIndex].mNumIndices == 3, "Mesh faces must be triangles!");
                m_indices.push_back(mesh->mFaces[faceIndex].mIndices[0]);
                m_indices.push_back(mesh->mFaces[faceIndex].mIndices[1]);
                m_indices.push_back(mesh->mFaces[faceIndex].mIndices[2]);
            }
        }

        TraverseNodes(scene->mRootNode);

        BufferDesc vertexBufferDesc{};
        vertexBufferDesc.Type = BufferType::eVertex;
        vertexBufferDesc.Size = sizeof(Vertex) * vertexCount;
        m_vertexBuffer = std::make_shared<Buffer>(vertexBufferDesc);

        BufferDesc indexBufferDesc{};
        indexBufferDesc.Type = BufferType::eIndex;
        indexBufferDesc.Size = sizeof(uint32_t) * indexCount;
        m_indexBuffer = std::make_shared<Buffer>(indexBufferDesc);

        // TODO: Load textures/materials

        const auto meshShader = ShaderLibrary::Get("PBR_Static");

        if (scene->HasMaterials())
        {
            GFX_INFO("  Materials: {}", scene->mNumMaterials);

            m_materials.resize(scene->mNumMaterials);

            for (uint32_t matIndex = 0; matIndex < scene->mNumMaterials; matIndex++)
            {
                auto aiMaterial = scene->mMaterials[matIndex];
                auto aiMaterialName = aiMaterial->GetName();
                GFX_INFO("  Material {}: {}", matIndex, aiMaterialName.data);

                auto mat = std::make_shared<Material>(meshShader);
                m_materials[matIndex] = mat;

                glm::vec3 ambientColor = { 0.8f, 0.8f, 0.8f };
                aiColor3D aiColor;
                if (aiMaterial->Get(AI_MATKEY_COLOR_AMBIENT, aiColor) == AI_SUCCESS) ambientColor = { aiColor.r, aiColor.g, aiColor.b };
                mat->Set("u_MaterialUniforms.Ambient", ambientColor);
                GFX_INFO("    Ambient = {}, {}, {}", aiColor.r, aiColor.g, aiColor.b);

                glm::vec3 diffuseColor = { 0.8f, 0.8f, 0.8f };
                if (aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor) == AI_SUCCESS) diffuseColor = { aiColor.r, aiColor.g, aiColor.b };
                mat->Set("u_MaterialUniforms.Diffuse", diffuseColor);
                GFX_INFO("    Diffuse = {}, {}, {}", aiColor.r, aiColor.g, aiColor.b);

                glm::vec3 specular = { 0.8f, 0.8f, 0.8f };
                if (aiMaterial->Get(AI_MATKEY_COLOR_SPECULAR, aiColor) == AI_SUCCESS) specular = { aiColor.r, aiColor.g, aiColor.b };
                mat->Set("u_MaterialUniforms.Specular", specular);
                GFX_INFO("    Specular = {}, {}, {}", aiColor.r, aiColor.g, aiColor.b);

                /*float shininess;
                float metalness;
                if (aiMaterial->Get(AI_MATKEY_SHININESS, shininess) != AI_SUCCESS) shininess = 80.0f;    // Default value
                if (aiMaterial->Get(AI_MATKEY_REFLECTIVITY, metalness) != AI_SUCCESS) metalness = 0.0f;  // Default value

                float roughness = 1.0f - glm::sqrt(shininess / 100.0f);
                GFX_INFO("    Metalness = {}", metalness);
                GFX_INFO("    Roughness = {}", roughness);

                mat->Set("u_MaterialUniforms.Metalness", metalness);
                mat->Set("u_MaterialUniforms.Roughness", roughness);*/
            }
        }
        else
        {
            GFX_INFO("  No materials. Creating a default one...");
            auto mat = std::make_shared<Material>(meshShader);
            mat->Set("u_MaterialUniforms.Ambient", { 0.8f, 0.8f, 0.8f });
            mat->Set("u_MaterialUniforms.Diffuse", { 0.8f, 0.8f, 0.8f });
            mat->Set("u_MaterialUniforms.Specular", { 0.8f, 0.8f, 0.8f });
        }

        GFX_INFO("  Mesh loaded: {} vertices, {} indices", vertexCount, indexCount);
    }

    void Mesh::TraverseNodes(aiNode* node, const glm::mat4& parentTransform, uint32_t level)
    {
        const auto transform = parentTransform * Mat4FromAssimpMat4(node->mTransformation);
        for (uint32_t i = 0; i < node->mNumMeshes; i++)
        {
            const auto mesh = node->mMeshes[i];
            auto& submesh = m_submeshes[mesh];
            submesh.Transform = transform;
        }

        for (uint32_t i = 0; i < node->mNumChildren; i++)
        {
            TraverseNodes(node->mChildren[i], transform, level + 1);
        }
    }
}  // namespace gfx
