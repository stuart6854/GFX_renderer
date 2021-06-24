//
// Created by stumi on 07/06/21.
//

#include <ExampleBase/Window.h>

#include <GFX/GFX.h>
#include <GFX/Debug.h>
#include <GFX/Resources/Primitives.h>
#include <GFX/Renderers/RendererForward.h>
#include <GFX/Resources/Texture.h>

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

        gfx::TextureDesc textureDesc;
        const auto woodTexture = std::make_shared<gfx::Texture>(renderer.GetDeviceContext(), "resources/textures/wood.png", textureDesc);

        auto plane = gfx::Primitives::CreatePlane(renderer.GetDeviceContext(), 10.0f);
        plane->GetMaterials().at(0)->Set("u_DiffuseTexture", woodTexture);

        //        auto mesh = renderer.LoadMesh("resources/models/triangle/Triangle.gltf");
        //        auto mesh = renderer.LoadMesh("resources/models/cube.obj");
        //        auto mesh = renderer.LoadMesh("resources/models/stanford-bunny.obj");
        // auto mesh = renderer.LoadMesh("resources/models/gun/scene.gltf");
        auto mesh = renderer.LoadMesh("resources/models/backpack/scene.gltf");

        auto proj = glm::perspective(glm::radians(60.0f), (float)window.GetWidth() / (float)window.GetHeight(), 0.01f, 100.0f);
        proj[1][1] *= -1.0f;

        auto view = glm::lookAt(glm::vec3{ 0, 7, -7 }, glm::vec3{ 0, 0, 0 }, glm::vec3{ 0, 1, 0 });

        glm::vec3 position = { 0.0f, 2.0f, 0.0f };
        glm::vec3 rotation = { 0.0f, 20.0f, 0.0f };
        glm::vec3 scale = glm::vec3(1, 1, 1) * 0.005f;

        gfx::LightEnvironment lightEnvironment{};
        lightEnvironment.DirectionalLights[0].Direction = glm::normalize(glm::vec3{ -0.2f, -1.0f, 0.3f });
        lightEnvironment.DirectionalLights[0].Color = { 1.0f, 1.0f, 1.0f };

        auto& pointLight = lightEnvironment.PointLights.emplace_back();
        pointLight.Position = { -1.0f, 1.0f, -1.0f };
        pointLight.Color = { 1.0f, 0.0f, 0.0f };
        pointLight.Constant = 1.0f;
        pointLight.Linear = 0.35f;
        pointLight.Quadratic = 0.44f;

        auto& pointLight2 = lightEnvironment.PointLights.emplace_back();
        pointLight2.Position = { 1.0f, 1.0f, -1.0f };
        pointLight2.Color = { 0.0f, 0.0f, 1.0f };
        pointLight2.Constant = 1.0f;
        pointLight2.Linear = 0.35f;
        pointLight2.Quadratic = 0.44f;

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
