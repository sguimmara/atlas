#ifndef ATLAS_VIEWER_VIEWER_HPP
#define ATLAS_VIEWER_VIEWER_HPP

#include "common.hpp"
#include "atlas/scene/scene.hpp"

namespace atlas::viewer
{
    class Viewer
    {
    public:
        Viewer();
        ~Viewer();

    private:
        std::shared_ptr<spdlog::logger> _log;
        GLFWwindow* _window;

        std::unique_ptr<scene::Scene> _scene;
    };
}

#endif