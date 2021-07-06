#pragma once

#include "Base.h"

#include <vector>

namespace gfx
{
    class MeshBuilder;
    class MeshImporter;
    class Mesh;
    class TextureBuilder;
    class TextureImporter;
    class Texture;
    class Shader;
    class Pipeline;
    class Material;

    class Renderer
    {
    public:
        void BeginFrame() = 0;
        void EndFrame() = 0;

        void Draw() = 0;
        void DrawDebug() = 0;

        auto CreateMesh(const MeshBuilder& builder) -> gfx::Mesh*;
        auto CreateMesh(const MeshImporter& importer) -> gfx::Mesh*;
        auto CreateTexture(const TextureBuilder& builder) -> gfx::Texture*;
        auto CreateTexture(const TextureImporter& importer) -> gfx::Texture*;
        // auto CreateShader() -> gfx::Shader*;
        // auto CreatePipeline() -> gfx::Pipeline*;
        // auto CreateMaterial() -> gfx::Material*;

        void Destroy(gfx::Mesh* mesh);
        void Destroy(gfx::Texture* texture);
        // void Destroy(gfx::Shader* shader);
        // void Destroy(gfx::Pipeline* pipeline);
        // void Destroy(gfx::Material* material);

    private:
    private:
        std::vector<OwnedPtr<gfx::Mesh>> m_meshes;
        std::vector<OwnedPtr<gfx::Texture>> m_textures;
    };
}
