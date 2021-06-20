//
// Created by stumi on 07/06/21.
//

#include <ExampleBase/Window.h>

#include <GFX/GFX.h>
#include <GFX/Debug.h>
#include <GFX/Renderers/RendererForward.h>
#include <GFX/Resources/Mesh.h>

#include <glm/gtc/matrix_transform.hpp>

int main(int argc, char** argv)
{
    GFX_INFO("Running example \"HelloForwardRenderer\"");

    gfx::Init();
    {
        example::Window window;

        gfx::RendererForward renderer;
        renderer.Init(window);

        //        auto mesh = renderer.LoadMesh("resources/forwardrenderer/triangle/Triangle.gltf");
        //        auto mesh = renderer.LoadMesh("resources/forwardrenderer/cube.obj");
        auto mesh = renderer.LoadMesh("resources/forwardrenderer/stanford-bunny.obj");

        auto proj = glm::perspective(glm::radians(60.0f), (float)window.GetWidth() / (float)window.GetHeight(), 0.01f, 1000.0f);
        proj[1][1] *= -1.0f;

        auto view = glm::lookAt(glm::vec3{ -3, 2, -3 }, glm::vec3{ 0, 0, 0 }, glm::vec3{ 0, 1, 0 });

        auto model = proj * view * glm::scale(glm::mat4(1.0f), glm::vec3{ 10, 10, 10 });

        while (!window.ShouldClose())
        {
            window.PollEvents();

            renderer.BeginScene();

            renderer.DrawMesh({ mesh.get(), model });

            renderer.EndScene();
        }
    }
    gfx::Shutdown();

    return 0;
}