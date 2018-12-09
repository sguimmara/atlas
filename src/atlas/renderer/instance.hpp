#ifndef ATLAS_RENDERER_INSTANCE_HPP_
#define ATLAS_RENDERER_INSTANCE_HPP_

#include "common.hpp"
#include "context.hpp"
#include "allocator.hpp"
#include "pipeline.hpp"
#include "uniformbuffer.hpp"

namespace atlas::renderer
{
    // Holds the Vulkan state: device(s), physical device(s), instance...
    // This class contains only static methods and fields.
    class Instance
    {
    public:
        // Initialize the instance to render on the provided window.
        static void initialize(GLFWwindow*);

        // Release all Vulkan resources associated with this instance.
        // This should be the last method to be called. A non-zero return value
        // indicates that an validation layer raised an error.
        static uint32_t terminate();

        // Handle the framebuffer resized event fired from GLFW.
        static void resizeWindow(GLFWwindow*, int width, int height);

        // The current rendering context associated with this instance.
        // May be null if the window is minimized or unable to render.
        static Context* context() noexcept;

        static std::unique_ptr<UniformBuffer> getGlobalPropertyBuffer();

        static void raiseValidationError(const std::string&);

        static void setShaderDirectory(const std::string&);
        static std::string shaderDirectory() noexcept;

        static vk::PhysicalDevice physicalDevice;
        static vk::Device device;

        static vk::Queue graphicsQueue;
        static vk::Queue presentQueue;
        static vk::Queue computeQueue;
        static vk::Queue transferQueue;
        static vk::RenderPass renderPass;
        static vk::DescriptorPool descriptorPool;

    private:
        static bool _debugMarkerSupported;
        static std::shared_ptr<spdlog::logger> _log;
        static VkDebugReportCallbackEXT _debugCallback;
        static vk::Instance _instance;
        static vk::SurfaceKHR _surface;
        static vk::SurfaceFormatKHR _surfaceFormat;
        static std::unique_ptr<Context> _context;
        static GLFWwindow* _window;
        static uint32_t _exitCode;
        static std::string _shaderDirectory;

        static void createInstance();
        static void setupDebugCallbacks();
        static void destroyDebugCallbacks();
        static void createSurface(GLFWwindow*);
        static void pickPhysicalDevice();
        static void createDevice();
        static void destroyDevice();
        static void createRenderPass();
        static void createContext();
    };
}

#endif // ATLAS_RENDERER_INSTANCE_HPP_