//
// Created by stumi on 19/06/21.
//

#include "GFX/Resources/Mesh.h"

#include "GFX/Debug.h"
#include "GFX/Resources/ResourceDescriptions.h"

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
            submesh.VertexCount = mesh->mNumVertices;
            submesh.IndexCount = mesh->mNumFaces * 3;

            GFX_INFO("  Submesh: {} vertices, {} indices", submesh.VertexCount, submesh.IndexCount);

            vertexCount += mesh->mNumVertices;
            indexCount += submesh.IndexCount;

            // TODO: Assert(mesh->HasPositions())
            // TODO: Assert(mesh->HasNormals())

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
                // TODO: Assert(mesh->mFaces[i].mNumIndices == 3)
                m_indices.push_back(mesh->mFaces[faceIndex].mIndices[0]);
                m_indices.push_back(mesh->mFaces[faceIndex].mIndices[1]);
                m_indices.push_back(mesh->mFaces[faceIndex].mIndices[2]);
            }
        }

        // TODO: Load textures/materials

        BufferDesc vertexBufferDesc{};
        vertexBufferDesc.Type = BufferType::eVertex;
        vertexBufferDesc.Size = sizeof(Vertex) * vertexCount;
        m_vertexBuffer = std::make_shared<Buffer>(vertexBufferDesc);

        BufferDesc indexBufferDesc{};
        indexBufferDesc.Type = BufferType::eIndex;
        indexBufferDesc.Size = sizeof(uint32_t) * indexCount;
        m_indexBuffer = std::make_shared<Buffer>(indexBufferDesc);

        GFX_INFO("  Mesh loaded: {} vertices, {} indices", vertexCount, indexCount);
    }

}  // namespace gfx