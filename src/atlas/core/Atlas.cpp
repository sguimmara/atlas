#include "Atlas.hpp"
#include "atlas/graphics/Mesh.hpp"
#include "atlas/graphics/Camera.hpp"
#include "atlas/graphics/debug/Point.hpp"
#include "atlas/graphics/debug/Axes.hpp"

#include "spdlog/sinks/stdout_color_sinks.h"

atlas::Atlas::Atlas()
{
    _log = spdlog::stdout_color_mt("atlas");

    _log->info("started");
#if DEBUG
    _log->set_level(spdlog::level::trace);
    _log->info("configuration is Debug");
#endif

    InitWindow();

    _renderer = new graphics::Renderer();

    _renderer->Setup(_window);

    _scene = new graphics::Scene();

    _scene->root()->add_child(new graphics::debug::Point(glm::vec3(0, 0, 0)));
    //_scene->root()->add_child(new graphics::debug::Axes());
    //_scene->root()->add_child(new graphics::Earth());
    _scene->root()->add_child(new graphics::Camera());
    _renderer->SetScene(_scene);

    glfwSetWindowUserPointer(_window, _renderer);

    _log->info("scene created");

    _renderer->Run();
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

    _log->debug("initialized window");
}
