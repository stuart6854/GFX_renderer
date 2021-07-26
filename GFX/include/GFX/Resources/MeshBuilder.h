#pragma once

#include "Vertex.h"

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include <vector>

namespace gfx
{
    class MeshBuilder
    {
    public:
        static auto CreatePlane(float size = 1.0f, uint32_t resolution = 1) -> MeshBuilder;
        static auto CreateCube(const glm::vec3& size = { 1.0f, 1.0f, 1.0f }) -> MeshBuilder;
        static auto CreateSphere(float radius = 0.5f) -> MeshBuilder;
        // static auto CreateCone() -> MeshBuilder;
        // static auto CreateCylinder() -> MeshBuilder;
        // static auto CreateTorus() -> MeshBuilder;

        auto AddVertex(const Vertex& position) -> uint32_t;

        // void AddLine(uint32_t a, uint32_t b);
        void AddTriangle(uint32_t a, uint32_t b, uint32_t c);

        auto GetVertex(uint32_t index) const -> const Vertex& { return m_vertices.at(index); }
        auto GetVertices() const -> const std::vector<Vertex>& { return m_vertices; }
        auto GetIndices() const -> const std::vector<uint32_t>& { return m_indices; }

    private:
        std::vector<Vertex> m_vertices = {};
        std::vector<uint32_t> m_indices = {};
    };
}
