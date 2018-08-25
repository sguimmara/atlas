#ifndef ATLAS_GRAPHICS_RENDERER_HPP
#define ATLAS_GRAPHICS_RENDERER_HPP

#include "AtlasGraphics.hpp"
#include "Framebuffer.hpp"
#include "Drawable.hpp"

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
            * @brief Executes the render loop.
            */
            void mainLoop();

            /**
            * @brief Initializes the renderer on the specified window.
            */
            void init(GLFWwindow * window);

            /**
            * @brief Warns the renderer the window size changed.
            */
            void onWindowResized(int newWidth, int newHeight);

            inline Framebuffer * framebuffer() const noexcept { return mFramebuffer; }
            inline VkCommandPool commandPool() const noexcept { return mVkCommandPool; }
            inline VkDevice device() const noexcept { return mVkDevice; }

            inline const VkClearValue* clearColor() const noexcept { return &mClearColor; }

            /* the duration of the last frame, in seconds. */
            float dt() const;

            uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        private:
            void createInstance();
            void destroyInstance();

            void setupDebugCallback();
            void createSurface(GLFWwindow* window);

            QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
            bool isDeviceSuitable(VkPhysicalDevice device, QueueFamilyIndices indices);
            bool checkSupportedImageFormat(VkFormat format);

            void selectPhysicalDevice();
            void createLogicalDevice();
            void destroyLogicalDevice();

            void createSemaphores();
            void destroySemaphores();

            void createCommandPool();
            void destroyCommandPool();

            void createFramebuffer();
            void checkFramebufferResized();
            void destroyFramebuffer();
            
            void renderFrame();
            void submitCommands(uint32_t imageIndex);

            std::shared_ptr<spdlog::logger> mLog;

            GLFWwindow* mWindow;
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
            VkClearValue mClearColor = { 0, 0, 0, 1.0f };

            Framebuffer* mFramebuffer = nullptr;

            std::chrono::milliseconds mFrameDuration;

            bool mWindowResized = false;
            uint32_t currentFrame = 0;

            Drawable * mDrawable;
        };
    }
}

#endif //!ATLAS_GRAPHICS_RENDERER_HPP