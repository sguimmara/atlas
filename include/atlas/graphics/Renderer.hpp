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
        struct QueueFamilyIndices
        {
            int graphicsFamily = -1;
            int presentFamily = -1;

            bool isComplete()
            {
                return graphicsFamily >= 0 && presentFamily >= 0;
            }
        };

        /**
        * @brief The graphics renderer.
        * @details The Renderer class draws 3D graphics on screen using the Vulkan API.
        */
        class Renderer
        {
        public:
            Renderer();
            ~Renderer();

            /**
            * @brief Initializes the renderer on the specified window.
            */
            void init(GLFWwindow * window);

        private:
            void create_instance();
            void destroy_instance();

            void setup_debug_callback();
            void create_surface(GLFWwindow* window);

            QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
            bool isDeviceSuitable(VkPhysicalDevice device, QueueFamilyIndices indices);
            bool checkSupportedImageFormat(VkFormat format);

            void pick_physical_device();
            void createLogicalDevice();
            void destroyLogicalDevice();

            void createSemaphores();
            void destroySemaphores();

            void createCommandPool();

            void destroyCommandPool();

            std::shared_ptr<spdlog::logger> mLog;

            /* Vulkan related fields */
            VkInstance mVkInstance = VK_NULL_HANDLE;
            VkDebugReportCallbackEXT mVkValidationCallback = VK_NULL_HANDLE;
            VkPhysicalDevice mVkPhysicalDevice = VK_NULL_HANDLE;
            VkDevice mVkDevice = VK_NULL_HANDLE;
            VkSurfaceKHR mVkSurface = VK_NULL_HANDLE;
            VkCommandPool mVkCommandPool = VK_NULL_HANDLE;
            QueueFamilyIndices mFamilyIndices;
            VkQueue mVkGraphicsQueue = VK_NULL_HANDLE;
            VkQueue mVkPresentQueue = VK_NULL_HANDLE;
            VkSemaphore mSemaphoreRenderFinished = VK_NULL_HANDLE;
            VkSemaphore mSemaphoreImageAvailable = VK_NULL_HANDLE;
        };
    }
}

#endif //!ATLAS_GRAPHICS_RENDERER_HPP