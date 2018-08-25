#include "Atlas.hpp"

#include "spdlog/sinks/stdout_color_sinks.h"

atlas::Atlas::Atlas() :
    mRenderer(graphics::Renderer())
{
    mLog = spdlog::stdout_color_mt("atlas");

    mLog->info("started");
#if DEBUG
    mLog->set_level(spdlog::level::trace);
    mLog->info("configuration is Debug");
#endif

    init_window();

    mRenderer.init(mWindow);

    mRenderer.mainLoop();
}

void atlas::Atlas::window_resized_callback(GLFWwindow* window, int width, int height)
{
    auto app = reinterpret_cast<atlas::Atlas*>(glfwGetWindowUserPointer(window));

    app->window_resized(width, height);
}

void atlas::Atlas::window_resized(int width, int height)
{
    mRenderer.onWindowResized(width, height);
}

/* initializes the GLFW window */
void atlas::Atlas::init_window()
{
    if (glfwInit() != GLFW_TRUE)
    {
        mLog->critical("GLFW failed to initialize.");
        std::terminate();
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    if (!(mWindow = glfwCreateWindow(800, 600, APP_NAME, nullptr, nullptr)))
    {
        throw std::runtime_error("GLFW failed to create window");
    }

    glfwSetWindowUserPointer(mWindow, this);
    glfwSetFramebufferSizeCallback(mWindow, window_resized_callback);
    
    mLog->debug("initialized window");
}
