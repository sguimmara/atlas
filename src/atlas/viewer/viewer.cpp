#include "viewer.hpp"

#include "atlas/renderer/instance.hpp"

using namespace atlas::viewer;
using namespace atlas::renderer;

Viewer::Viewer()
{
    _log = spdlog::stdout_color_mt(APP_NAME);
    _log->info("initialize");
#if DEBUG
    spdlog::set_level(spdlog::level::debug);
#endif

    if (glfwInit() != GLFW_TRUE)
    {
        std::runtime_error("could not initialize GLFW");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    _window = glfwCreateWindow(800, 600, APP_NAME, nullptr, nullptr);

    _log->debug("create window");

    Instance::initialize(_window);
    Instance::setShaderDirectory("C:/Users/sguimmara/Documents/work/c++/atlas4/.build/bin/shaders/");

    _scene = std::make_unique<scene::Scene>("main");
}

Viewer::~Viewer()
{
    _scene = nullptr;

    Instance::terminate();

    glfwDestroyWindow(_window);

    _log->info("exit");
}