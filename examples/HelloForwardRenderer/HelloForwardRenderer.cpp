//
// Created by stumi on 07/06/21.
//

#include <ExampleBase/Window.h>

#include <GFX/GFX.h>
#include <GFX/Debug.h>
#include <GFX/Renderers/RendererForward.h>
#include <GFX/Resources/Mesh.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

int main(int argc, char** argv)
{
    GFX_INFO("Running example \"HelloForwardRenderer\"");

    gfx::Init();
    {
        example::Window window("HelloForwardRenderer", 1080, 720);

        gfx::RendererForward renderer;
        renderer.Init(window);

        //        auto mesh = renderer.LoadMesh("resources/forwardrenderer/triangle/Triangle.gltf");
        //        auto mesh = renderer.LoadMesh("resources/forwardrenderer/cube.obj");
        //        auto mesh = renderer.LoadMesh("resources/forwardrenderer/stanford-bunny.obj");
        auto mesh = renderer.LoadMesh("resources/forwardrenderer/gun/scene.gltf");

        auto proj = glm::perspective(glm::radians(60.0f), (float)window.GetWidth() / (float)window.GetHeight(), 0.01f, 1000.0f);
        proj[1][1] *= -1.0f;

        auto view = glm::lookAt(glm::vec3{ 0, 2, -2 }, glm::vec3{ 0, 0, 0 }, glm::vec3{ 0, 1, 0 });

        auto rot = glm::eulerAngleXYZ(glm::radians(90.0f), glm::radians(0.0f), glm::radians(90.0f));
        auto model = glm::translate(glm::mat4(1.0f), glm::vec3{ -1, 0, 0 }) * rot * glm::scale(glm::mat4(1.0f), glm::vec3{ 0.025f, 0.025f, 0.025f });

        gfx::LightEnvironment lightEnvironment{};
        lightEnvironment.DirectionalLights[0].Direction = glm::normalize(glm::vec3{ -0.8f, -1, 0.9f });
        lightEnvironment.DirectionalLights[0].Multiplier = 1.0f;
        //        lightEnvironment.DirectionalLights[0].Radiance = glm::vec3{ -1, -1, 1 };

        while (!window.ShouldClose())
        {
            window.PollEvents();

            renderer.BeginScene({ proj, view }, lightEnvironment);

            renderer.DrawMesh({ mesh.get(), model });

            renderer.EndScene();
        }
    }
    gfx::Shutdown();

    return 0;
}