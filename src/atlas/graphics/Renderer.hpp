#ifndef ATLAS_GRAPHICS_RENDERER_HPP
#define ATLAS_GRAPHICS_RENDERER_HPP

#include "AtlasGraphics.hpp"
#include "RenderTarget.hpp"

namespace atlas
{
    namespace graphics
    {
        class Drawable;

        class Renderer
        {
        public:
            Renderer();
            void Setup(GLFWwindow* window);
            ~Renderer();

            inline vk::Device device() const noexcept { return _device; }
            inline vk::PhysicalDevice gpu() const noexcept { return _gpu; }
            inline vk::CommandPool pool() const noexcept { return _commandPool; }
            inline vk::Extent2D extent() const noexcept { return _extent; }
            inline vk::Viewport viewport() const noexcept { return _viewport; }
            inline vk::RenderPass renderPass() const noexcept { return _renderPass; }
            inline size_t swapchainSize() const noexcept { return _renderTargets.size(); }

            uint32_t FindMemoryType(vk::PhysicalDevice gpu, uint32_t typeFilter, vk::MemoryPropertyFlags properties);
            void CreateBuffer(uint32_t size, char* data, vk::Buffer * buffer, vk::DeviceMemory * memory, vk::BufferUsageFlags usage);

            void Run();

        private:
            void CreateInstance();
            void DestroyInstance();

            void SetupDebugCallbacks();
            void CreateSurface();
            void SelectGpu();
            bool CheckExtensionSupport(vk::PhysicalDevice gpu);

            void CreateDevice();
            void DestroyDevice();

            // swapchain management
            vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>);
            vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR&, GLFWwindow*);
            vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>&);
            void CreateSwapchain();
            void DestroySwapchain();

            void CreateCommandPool();
            void DestroyCommandPool();
            void CreateCommandBuffers();
            void UpdateCommandBuffers();

            void RenderFrame();
            void SubmitFrame(RenderTarget image, vk::CommandBuffer buffer);

            std::shared_ptr<spdlog::logger> _log;

            vk::Instance _instance;
            vk::PhysicalDevice _gpu;
            vk::Device _device;
            vk::SwapchainKHR _swapchain;
            uint32_t _swapWaitTimeout = 1000;
            vk::SurfaceFormatKHR _swapchainFormat;
            vk::RenderPass _renderPass;
            vk::SurfaceKHR _surface;
            vk::CommandPool _commandPool;
            vk::Queue _presentQueue;
            vk::Queue _graphicsQueue;
            vk::Extent2D _extent;
            vk::Viewport _viewport;
            VkDebugReportCallbackEXT _debugCallback;

            uint32_t _presentFamily;
            uint32_t _graphicsFamily;
            uint32_t _imageIndex;

            std::vector<const char*> _enabledExtensions;
            std::vector<RenderTarget> _renderTargets;
            std::vector<vk::CommandBuffer> _commandBuffers;

            Drawable* _drawable;

            GLFWwindow * _window;
        };
    }
}

#endif