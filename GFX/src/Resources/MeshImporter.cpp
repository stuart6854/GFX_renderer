#include "GFX/Resources/MeshImporter.h"

#include "GFX/Debug.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <filesystem>

namespace gfx
{
    const uint32_t MeshImportFlags = aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_CalcTangentSpace | aiProcess_MakeLeftHanded;

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

    MeshImporter::MeshImporter(const std::string& filename)
        : m_filename(filename)
    {
        LoadMesh(filename);
    }

    void MeshImporter::LoadSubMeshes(const aiScene* scene)
    {
        uint32_t vertexCount = 0;
        uint32_t indexCount = 0;

        m_subMeshes.reserve(scene->mNumMeshes);
        for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++)
        {
            auto* mesh = scene->mMeshes[meshIndex];

            auto& submesh = m_subMeshes.emplace_back();
            submesh.BaseVertex = vertexCount;
            submesh.BaseIndex = indexCount;
            submesh.MaterialIndex = mesh->mMaterialIndex;
            submesh.VertexCount = mesh->mNumVertices;
            submesh.IndexCount = mesh->mNumFaces * 3;

            GFX_TRACE("  Submesh: {} vertices, {} indices", submesh.VertexCount, submesh.IndexCount);

            vertexCount += submesh.VertexCount;
            indexCount += submesh.IndexCount;

            GFX_ASSERT(mesh->HasPositions(), "Mesh must have positions!");

            // Vertices
            for (uint32_t vertIndex = 0; vertIndex < mesh->mNumVertices; vertIndex++)
            {
                auto& vertex = m_vertices.emplace_back();
                vertex.Position = { mesh->mVertices[vertIndex].x, mesh->mVertices[vertIndex].y, mesh->mVertices[vertIndex].z };

                if (mesh->HasNormals())
                    vertex.Normal = { mesh->mNormals[vertIndex].x, mesh->mNormals[vertIndex].y, mesh->mNormals[vertIndex].z };

                if (mesh->HasTextureCoords(0))
                    vertex.TexCoord = { mesh->mTextureCoords[0][vertIndex].x, mesh->mTextureCoords[0][vertIndex].y };

                if (mesh->HasTangentsAndBitangents())
                {
                    vertex.Tangent = { mesh->mTangents[vertIndex].x, mesh->mTangents[vertIndex].y, mesh->mTangents[vertIndex].z };
                    vertex.BiTangent = { mesh->mBitangents[vertIndex].x, mesh->mBitangents[vertIndex].y, mesh->mBitangents[vertIndex].z };
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
    }

    void MeshImporter::LoadMaterials(const aiScene* scene)
    {
        if (!scene->HasMaterials())
            return;

        m_materials.resize(scene->mNumMaterials);

        for (uint32_t matIndex = 0; matIndex < scene->mNumMaterials; matIndex++)
        {
            auto* aiMat = scene->mMaterials[matIndex];

            auto& materialDef = m_materials[matIndex];

            materialDef.Name = aiMat->GetName().C_Str();
            GFX_TRACE("Material {}", materialDef.Name);

            aiColor3D color;
            // Ambient Color
            {
                aiMat->Get(AI_MATKEY_COLOR_AMBIENT, color);
                materialDef.AmbientColor = { color.r, color.g, color.b };
                GFX_TRACE("  Ambient Color = {}", materialDef.AmbientColor);
            }
            // Diffuse Color
            {
                aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
                materialDef.DiffuseColor = { color.r, color.g, color.b };
                GFX_TRACE("  Diffuse Color = {}", materialDef.DiffuseColor);
            }
            // Specular Color
            {
                aiMat->Get(AI_MATKEY_COLOR_SPECULAR, color);
                materialDef.SpecularColor = { color.r, color.g, color.b };
                GFX_TRACE("  Specular Color = {}", materialDef.SpecularColor);
            }
            // Shininess value
            {
                aiMat->Get(AI_MATKEY_SHININESS, materialDef.Shininess);
                GFX_TRACE("  Shininess = {}", materialDef.Shininess);
            }

            const auto modelDir = std::filesystem::path(m_filename).parent_path();

            aiString aiTexturePath;
            // Ambient texture
            if (aiMat->GetTexture(aiTextureType_AMBIENT, 0, &aiTexturePath) == AI_SUCCESS)
            {
                std::string texPath = modelDir.string().append("/").append(aiTexturePath.C_Str());
                materialDef.AmbientTexture = texPath;
                GFX_TRACE("  Ambient Texture = {}", materialDef.AmbientTexture);
            }
            // Diffuse texture
            if (aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &aiTexturePath) == AI_SUCCESS)
            {
                std::string texPath = modelDir.string().append("/").append(aiTexturePath.C_Str());
                materialDef.DiffuseTexture = texPath;
                GFX_TRACE("  Diffuse Texture = {}", materialDef.DiffuseTexture);
            }
            // Specular texture
            if (aiMat->GetTexture(aiTextureType_SPECULAR, 0, &aiTexturePath) == AI_SUCCESS)
            {
                std::string texPath = modelDir.string().append("/").append(aiTexturePath.C_Str());
                materialDef.SpecularTexture = texPath;
                GFX_TRACE("  Specular Texture = {}", materialDef.SpecularTexture);
            }

            // Normal Map texture
            if (aiMat->GetTexture(aiTextureType_NORMALS, 0, &aiTexturePath) == AI_SUCCESS)
            {
                std::string texPath = modelDir.string().append("/").append(aiTexturePath.C_Str());
                materialDef.NormalMap = texPath;
                GFX_TRACE("  Normal Map = {}", materialDef.NormalMap);
            }
        }
    }

    void MeshImporter::LoadMesh(const std::string& filename)
    {
        Assimp::Importer importer;
        auto* scene = importer.ReadFile(filename, MeshImportFlags);
        if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr)
        {
            GFX_ERROR("Failed to load mesh: {}", importer.GetErrorString());
            return;
        }

        LoadSubMeshes(scene);
        LoadMaterials(scene);

        GFX_TRACE("Mesh loaded: {} meshes, {} materials", scene->mNumMeshes, scene->mNumMaterials);
    }

    void MeshImporter::TraverseNodes(aiNode* node, const glm::mat4& parentTransform, uint32_t level)
    {
        const auto transform = parentTransform * Mat4FromAssimpMat4(node->mTransformation);
        for (uint32_t i = 0; i < node->mNumMeshes; i++)
        {
            const auto mesh = node->mMeshes[i];
            auto& submesh = m_subMeshes[mesh];
            submesh.Transform = transform;
        }

        for (uint32_t i = 0; i < node->mNumChildren; i++)
        {
            TraverseNodes(node->mChildren[i], transform, level + 1);
        }
    }
}
