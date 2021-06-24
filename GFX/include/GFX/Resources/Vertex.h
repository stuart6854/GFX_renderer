//
// Created by stumi on 07/06/21.
//

#ifndef PERSONAL_RENDERER_VERTEX_H
#define PERSONAL_RENDERER_VERTEX_H

#include <glm/glm.hpp>

namespace gfx
{
    struct Vertex
    {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoord;
        glm::vec3 Tangent;
        glm::vec3 Bitangent;
    };

}  // namespace gfx

#endif  // PERSONAL_RENDERER_VERTEX_H
