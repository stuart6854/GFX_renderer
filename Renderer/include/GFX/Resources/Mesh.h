//
// Created by stumi on 07/06/21.
//

#ifndef PERSONAL_RENDERER_MESH_H
#define PERSONAL_RENDERER_MESH_H

#include "Vertex.h"

namespace gfx
{
    class Mesh
    {
    public:
        Mesh();
        ~Mesh();

    private:
        std::vector<Vertex> m_vertices;
        std::vector<uint32_t> m_indices;

    };
}  // namespace gfx

#endif  // PERSONAL_RENDERER_MESH_H
