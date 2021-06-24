//
// Created by stumi on 07/06/21.
//

#include <ExampleBase/Window.h>

#include <GFX/GFX.h>
#include <GFX/Debug.h>
#include <GFX/Resources/Primitives.h>
#include <GFX/Renderers/RendererForward.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <chrono>

int main(int argc, char** argv)
{
    GFX_INFO("Running example \"HelloForwardRenderer\"");

    gfx::Init();
    {
        example::Window window("HelloForwardRenderer", 1080, 720);

        gfx::RendererForward renderer;
        renderer.Init(window);

        auto plane = gfx::Primitives::CreatePlane(renderer.GetDeviceContext(), 10.0f);

        //        auto mesh = renderer.LoadMesh("resources/models/triangle/Triangle.gltf");
        //        auto mesh = renderer.LoadMesh("resources/models/cube.obj");
        //        auto mesh = renderer.LoadMesh("resources/models/stanford-bunny.obj");
        // auto mesh = renderer.LoadMesh("resources/models/gun/scene.gltf");
        auto mesh = renderer.LoadMesh("resources/models/backpack/scene.gltf");

        auto proj = glm::perspective(glm::radians(60.0f), (float)window.GetWidth() / (float)window.GetHeight(), 0.01f, 100.0f);
        proj[1][1] *= -1.0f;

        auto view = glm::lookAt(glm::vec3{ 0, 2, -6 }, glm::vec3{ 0, 0, 0 }, glm::vec3{ 0, 1, 0 });

        glm::vec3 position = { 0.0f, 0.0f, 0.0f };
        glm::vec3 rotation = { 0.0f, 20.0f, 0.0f };
        glm::vec3 scale = glm::vec3(1, 1, 1) * 0.01f;

        gfx::LightEnvironment lightEnvironment{};
        lightEnvironment.DirectionalLights[0].Direction = glm::normalize(glm::vec3{ -0.2f, -1.0f, 0.3f });
        lightEnvironment.DirectionalLights[0].Multiplier = 1.0f;
        //        lightEnvironment.DirectionalLights[0].Radiance = glm::vec3{ -1, -1, 1 };

        using clock = std::chrono::high_resolution_clock;
        auto last = clock::now();
        auto delta = 0.0f;
        while (!window.ShouldClose())
        {
            {
                auto now = clock::now();
                using ms = std::chrono::duration<float, std::milli>;
                delta = std::chrono::duration_cast<ms>(now - last).count() / 1000.0f;
                last = now;
            }

            window.PollEvents();

            renderer.BeginScene({ proj, view }, lightEnvironment);

            renderer.DrawMesh({ plane.get() });

            {
                rotation.y += 20.0f * delta;

                const auto rot = glm::eulerAngleXYZ(glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z));
                const auto model = glm::translate(glm::mat4(1.0f), position) * rot * glm::scale(glm::mat4(1.0f), scale);

                renderer.DrawMesh({ mesh.get(), model });
            }

            renderer.EndScene();
        }
    }
    gfx::Shutdown();

    return 0;
}
