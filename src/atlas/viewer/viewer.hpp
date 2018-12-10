#ifndef ATLAS_VIEWER_VIEWER_HPP
#define ATLAS_VIEWER_VIEWER_HPP

#include "common.hpp"
#include "parameters.hpp"
#include "atlas/scene/scene.hpp"
#include "atlas/scene/time.hpp"
#include <chrono>

namespace atlas::viewer
{
    using namespace atlas::scene;

    class Viewer
    {
    public:
        Viewer(Parameters);

        // compute and render a single frame
        void update();

        // setup the main loop, calling update() indefinitely.
        void run();
        ~Viewer();

    private:
        std::chrono::time_point<std::chrono::steady_clock> _sessionStart;
        Parameters _parameters;
        std::shared_ptr<spdlog::logger> _log;
        GLFWwindow* _window;

        std::unique_ptr<scene::Scene> _scene;
        Time time;
    };
}

#endif