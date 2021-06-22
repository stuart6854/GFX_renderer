//
// Created by stumi on 19/06/21.
//

#include "GFX/Resources/Mesh.h"

#include "GFX/Debug.h"
#include "GFX/Resources/Shader.h"
#include "GFX/Resources/ResourceDescriptions.h"
#include "GFX/Utility/Color.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace gfx
{
    static const uint32_t s_meshImportFlags = aiProcess_Triangulate | aiProcess_GenNormals;

    Mesh::Mesh(const std::string& path) { LoadMesh(path); }

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

        BufferDesc vertexBufferDesc{};
        vertexBufferDesc.Type = BufferType::eVertex;
        vertexBufferDesc.Size = sizeof(Vertex) * vertexCount;
        m_vertexBuffer = std::make_shared<Buffer>(vertexBufferDesc);

        BufferDesc indexBufferDesc{};
        indexBufferDesc.Type = BufferType::eIndex;
        indexBufferDesc.Size = sizeof(uint32_t) * indexCount;
        m_indexBuffer = std::make_shared<Buffer>(indexBufferDesc);

        // TODO: Load textures/materials

        // TODO: Some sort of shader library
        //         auto meshShader = GetMeshShader();
        auto meshShader = std::make_shared<Shader>("resources/PBR_Static.glsl");

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

                glm::vec3 albedoColor = { 0.8f, 0.8f, 0.8f };
                aiColor3D aiColor;
                if (aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor) == AI_SUCCESS) albedoColor = { aiColor.r, aiColor.g, aiColor.b };

                mat->Set("u_MaterialUniforms.AlbedoColor", albedoColor);

                GFX_INFO("    Albedo = {}, {}, {}", aiColor.r, aiColor.g, aiColor.b);
            }
        }
        else
        {
            GFX_INFO("  No materials. Creating a default one...");

            auto mat = std::make_shared<Material>(meshShader);
            mat->Set("u_MaterialUniforms.AlbedoColor", Color(0.8f, 0.1f, 0.3f));
        }

        GFX_INFO("  Mesh loaded: {} vertices, {} indices", vertexCount, indexCount);
    }

}  // namespace gfx