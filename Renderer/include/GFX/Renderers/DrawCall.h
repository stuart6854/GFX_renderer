//
// Created by stumi on 19/06/21.
//

#ifndef PERSONAL_RENDERER_DRAWCALL_H
#define PERSONAL_RENDERER_DRAWCALL_H

#include <glm/mat4x4.hpp>

namespace gfx
{
    class Mesh;
    class Pipeline;

    struct DrawCall
    {
        Mesh* mesh = nullptr;
        //        Material* overrideMaterial;
        glm::mat4 transform = glm::mat4(1.0f);
    };

}  // namespace gfx

#endif  // PERSONAL_RENDERER_DRAWCALL_H
