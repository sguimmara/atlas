#include "Atlas.hpp"

#include "spdlog/sinks/stdout_color_sinks.h"

atlas::Atlas::Atlas() :
    _renderer(graphics::Renderer())
{
    _log = spdlog::stdout_color_mt("atlas");

    _log->info("started");
#if DEBUG
    _log->set_level(spdlog::level::trace);
    _log->info("configuration is Debug");
#endif

    InitWindow();

    _renderer.Setup(_window);
    _renderer.Run();
}

void atlas::Atlas::WindowResizedCallback(GLFWwindow* window, int width, int height)
{
    auto app = reinterpret_cast<atlas::Atlas*>(glfwGetWindowUserPointer(window));

    app->WindowResized(width, height);
}

void atlas::Atlas::WindowResized(int width, int height)
{
    //FIXME
    // _renderer.onWindowResized(width, height);
}

/* initializes the GLFW window */
void atlas::Atlas::InitWindow()
{
    if (glfwInit() != GLFW_TRUE)
    {
        _log->critical("GLFW failed to initialize.");
        std::terminate();
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    if (!(_window = glfwCreateWindow(800, 600, APP_NAME, nullptr, nullptr)))
    {
        throw std::runtime_error("GLFW failed to create window");
    }

    glfwSetWindowUserPointer(_window, this);
    glfwSetFramebufferSizeCallback(_window, WindowResizedCallback);
    
    _log->debug("initialized window");
}
