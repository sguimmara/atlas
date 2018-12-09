#include "atlas/renderer/instance.hpp"
#include "atlas/core/quadtree.hpp"
#include "atlas/renderer/pipeline.hpp"
#include "atlas/renderer/mesh.hpp"
#include "atlas/scene/entity.hpp"
#include "atlas/scene/scene.hpp"
#include "atlas/scene/meshbuilder.hpp"
#include "atlas/scene/time.hpp"
#include "atlas/viewer/viewer.hpp"
#include "atlas/renderer/material.hpp"
#include "atlas/renderer/texture.hpp"
#include "atlas/renderer/globalproperties.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <GLFW/glfw3.h>
#include <chrono>

using namespace atlas::core;
using namespace atlas::scene;
using namespace atlas::renderer;

int main2(int argc, char* argv[])
{
    atlas::viewer::Viewer viewer;

    atlas::renderer::Texture tex("C:/Users/sguimmara/Pictures/casimir.jpg");

    return 0;
}
int main(int argc, char* argv[])
{
    spdlog::set_level(spdlog::level::trace);

    if (glfwInit() != GLFW_TRUE)
    {
        std::runtime_error("could not initialize GLFW");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWwindow* window = glfwCreateWindow(800, 600, "test", nullptr, nullptr);

    Time time;

    Instance::initialize(window);
    {
        Instance::setShaderDirectory("C:/Users/sguimmara/Documents/work/c++/atlas4/.build/bin/shaders/");

        Scene scene("main");

        auto sessionStart = std::chrono::steady_clock::now();

        while (!glfwWindowShouldClose(window))
        {
            auto frameStart = std::chrono::steady_clock::now();

            glfwPollEvents();

            // TODO framerate limiting
            std::this_thread::sleep_for(std::chrono::milliseconds(16));

            auto now = std::chrono::steady_clock::now();
            time.elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - sessionStart);
            time.deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - frameStart);
            time.frame++;

            scene.render(time);
        }
    }
    uint32_t exitCode = Instance::terminate();

    glfwDestroyWindow(window);

    return exitCode;
}