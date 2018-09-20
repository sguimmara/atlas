#ifndef ATLAS_HPP
#define ATLAS_HPP

#include "atlas/graphics/AtlasGraphics.hpp"
#include "atlas/graphics/Renderer.hpp"
#include "atlas/graphics/Scene.hpp"
#include "atlas/graphics/Camera.hpp"
#include "atlas/graphics/Earth.hpp"

namespace atlas
{
    /**
     * @brief Encapsulate the Atlas application.
     * @details The Atlas class contains the modules required to run an Atlas application, such
                as renderer, layer manager, and so on.
     */
    class Atlas
    {
    public:
        Atlas();
        ~Atlas();

        inline graphics::Scene* scene() const noexcept { return _scene; };

    private:
        static void WindowResizedCallback(GLFWwindow* window, int width, int height);

        void InitWindow();
        void WindowResized(int width, int height);

        std::shared_ptr<spdlog::logger> _log;

        graphics::Renderer* _renderer;

        GLFWwindow * _window;

        graphics::Scene* _scene;
    };
}
#endif //!ATLAS_HPP