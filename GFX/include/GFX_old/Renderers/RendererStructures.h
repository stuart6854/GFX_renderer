//
// Created by stumi on 20/06/21.
//

#ifndef PERSONAL_RENDERER_RENDERERSTRUCTURES_H
#define PERSONAL_RENDERER_RENDERERSTRUCTURES_H

#include <glm/glm.hpp>

#include <vector>

namespace gfxOld
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

    struct DirectionalLight
    {
        glm::vec3 Position = { 0.0f, 1.0f, 0.0f };
        glm::vec3 Target = { 0.0f, 0.0f, 0.0f };
        glm::vec3 Color = { 0.0f, 0.0f, 0.0f };
        bool CastShadows = true;
    };

    struct PointLight
    {
        glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
        float Padding = 0.0f;
        glm::vec3 Color = { 0.0f, 0.0f, 0.0f };
        float Constant = 0.001f;
        float Linear = 25.0f;
        float Quadratic = 1.f;
        float Padding2 = 0.1f;
        bool CastsShadows = true;
        char Padding3[3]{ 0, 0, 0 };
    };

    struct LightEnvironment
    {
        DirectionalLight DirectionalLights[1];
        std::vector<PointLight> PointLights;
    };

}  // namespace gfxOld

#endif  // PERSONAL_RENDERER_RENDERERSTRUCTURES_H
