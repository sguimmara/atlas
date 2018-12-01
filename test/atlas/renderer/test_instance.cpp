#include "atlas/renderer/instance.hpp"
#include "atlas/renderer/pipeline.hpp"
#include <GLFW/glfw3.h>

int main(int argc, char* argv[])
{
    if (glfwInit() != GLFW_TRUE)
    {
        std::runtime_error("could not initialize GLFW");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWwindow* window = glfwCreateWindow(800, 600, "test", nullptr, nullptr);

    atlas::renderer::Instance::initialize(window);
    atlas::renderer::Instance::setShaderDirectory("C:/Users/sguimmara/Documents/work/c++/atlas4/.build/bin/shaders/");

    atlas::renderer::Pipeline::create(R"%(
        {
            "name": "test",
            "vertex": "default.vert.spv",
            "fragment": "default.frag.spv",
            "rasterizer":{
                "cullMode": "front",
                "frontFace": "ccw"
            }
        })%");
    atlas::renderer::Pipeline::get("test");

    auto framesToRender = 10;
    while (!glfwWindowShouldClose(window) && framesToRender > 0)
    {
        glfwPollEvents();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));

        auto const ctx = atlas::renderer::Instance::context();

        // the context can be null if the window is invalid (e.g minimized).
        if (ctx)
        {
            // render empty frames (only clear color)
            ctx->beginFrame();
            ctx->endFrame();
        }

        framesToRender--;
    }

    uint32_t exitCode = atlas::renderer::Instance::terminate();

    glfwDestroyWindow(window);

    return exitCode;
}