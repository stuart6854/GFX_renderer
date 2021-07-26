#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace gfx
{
    struct Vertex
    {
        glm::vec3 Position = { 0, 0, 0 };
        glm::vec3 Normal = { 0, 0, 0 };
        glm::vec2 TexCoord = { 0, 0 };
        glm::vec3 Tangent = { 0, 0, 0 };
        glm::vec3 BiTangent = { 0, 0, 0 };
    };
}
