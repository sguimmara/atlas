#ifndef ATLAS_GRAPHICS_RENDERER_HPP
#define ATLAS_GRAPHICS_RENDERER_HPP

#include "spdlog/spdlog.h"

#include "spdlog/sinks/stdout_color_sinks.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace atlas
{
    namespace graphics
    {
        /**
        * @brief The graphics renderer.
        * @details The Renderer class draws 3D graphics on screen using the Vulkan API.
        */
        class Renderer
        {
        public:
            Renderer();

            /**
            * @brief Initializes the renderer on the specified window.
            */
            void init(GLFWwindow * window);

        private:
            void create_instance();

            std::shared_ptr<spdlog::logger> mLog;

            /* Vulkan related fields */
            VkInstance instance = VK_NULL_HANDLE;
        };
    }
}

#endif //!ATLAS_GRAPHICS_RENDERER_HPP