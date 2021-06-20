//
// Created by stumi on 20/06/21.
//

#ifndef PERSONAL_RENDERER_RENDERERSTRUCTURES_H
#define PERSONAL_RENDERER_RENDERERSTRUCTURES_H

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

    struct Camera
    {
        glm::mat4 ProjectionMatrix;
        glm::mat4 ViewMatrix;
    };

}  // namespace gfx

#endif  // PERSONAL_RENDERER_RENDERERSTRUCTURES_H
