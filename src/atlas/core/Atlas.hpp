#ifndef ATLAS_HPP
#define ATLAS_HPP

#include "atlas/graphics/AtlasGraphics.hpp"
#include "atlas/graphics/Renderer.hpp"

namespace atlas
{
    /**
     * @brief Encapsulate the Atlas application.
     * @details The Atlas class contains the modules required to run an Atlas application, such
                as renderer, layer manager, and so on.
     * 
     */
    class Atlas
    {
    public:
        Atlas();

    private:
        static void window_resized_callback(GLFWwindow* window, int width, int height);

        void init_window();
        void window_resized(int width, int height);

        std::shared_ptr<spdlog::logger> mLog;

        graphics::Renderer mRenderer;

        GLFWwindow * mWindow;

    };
}
#endif //!ATLAS_HPP