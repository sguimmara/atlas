#include "Atlas.hpp"
#include "atlas/core/Math.hpp"
#include "atlas/graphics/Mesh.hpp"
#include "atlas/graphics/Camera.hpp"
#include "atlas/graphics/objects/Earth.hpp"

#include "spdlog/sinks/stdout_color_sinks.h"

using namespace glm;
using namespace atlas::graphics;
using namespace atlas::graphics::objects;
using namespace atlas::core;

atlas::Atlas::Atlas()
{
    _log = spdlog::stdout_color_mt("atlas");

    _log->info("started");
#if DEBUG
    _log->set_level(spdlog::level::trace);
    _log->info("configuration is Debug");
#endif

    InitWindow();

    _renderer = new Renderer();

    _renderer->Setup(_window);

    _scene = new Scene();

    _scene->root()->add_child(new Earth(Ellipsoid::GRS80, 1));
    _scene->root()->add_child(new Camera());
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
