#ifndef ATLAS_GRAPHICS_RENDERER_HPP
#define ATLAS_GRAPHICS_RENDERER_HPP

#include "AtlasGraphics.hpp"
#include "RenderTarget.hpp"
#include "Scene.hpp"
#include "Node.hpp"

namespace atlas
{
    namespace graphics
    {
        class Camera;
        class Mesh;

        class Renderer
        {
        public:
            Renderer();
            void Setup(GLFWwindow* window);
            void SetScene(Scene* scene);
            void DestroyScene();
            ~Renderer();

            static vk::Device device;
            static vk::PhysicalDevice gpu;
            static vk::Viewport viewport;
            static vk::Extent2D extent;
            static Renderer* current;

            inline vk::CommandPool pool() const noexcept { return _commandPool; }
            inline vk::RenderPass renderPass() const noexcept { return _renderPass; }
            inline size_t swapchainSize() const noexcept { return _renderTargets.size(); }

            void OnWindowResized();
            void ProcessKeyEvents(int key, int scancode, int action, int mods);
            void ProcessScrollEvents(double x, double y);
            void CopyBufferToImage(uint32_t width, uint32_t height, vk::Buffer stage, vk::Image image);
            void TransitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
            vk::CommandBuffer BeginSingleTimeCommand();
            void EndSingleTimeCommand(vk::CommandBuffer);
            uint32_t GetMemoryIndex(vk::MemoryRequirements requirements, vk::MemoryPropertyFlags flags);
            uint32_t FindMemoryType(vk::PhysicalDevice gpu, uint32_t typeFilter, vk::MemoryPropertyFlags properties);
            void CreateBuffer(vk::DeviceSize size, void* data, vk::Buffer * buffer, vk::DeviceMemory * memory, vk::BufferUsageFlags usage);

            void Run();

        private:
            void CreateInstance();
            void DestroyInstance();

            void SetupDebugCallbacks();
            void CreateSurface();
            void SelectGpu();
            bool CheckExtensionSupport(vk::PhysicalDevice gpu);

            void CreateDevice();
            void CheckFeatures();
            void DestroyDevice();

            // swapchain management
            vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>);
            vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR&, GLFWwindow*);
            vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>&);
            void CreateSwapchain();
            void DestroySwapchain();
            void ResizeViewport();
            void UpdateViewportAndExtent();

            void CreateDepthResources();
            void DestroyDepthResources();

            void CreateCommandPool();
            void DestroyCommandPool();
            void CreateCommandBuffers();
            void UpdateCommandBuffers();

            void RenderFrame();
            void PostFrame(double);
            void ApplyTransformations(Scene& scene);
            void RenderScene(Scene& scene, Camera& camera, vk::CommandBuffer& cmdBuffer);
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
            vk::Image _depthImage;
            vk::ImageView _depthImageView;
            vk::DeviceMemory _depthImageMemory;
            VkDebugReportCallbackEXT _debugCallback;

            uint32_t _presentFamily;
            uint32_t _graphicsFamily;
            uint32_t _imageIndex;

            std::vector<const char*> _enabledExtensions;
            std::vector<RenderTarget> _renderTargets;
            std::vector<vk::CommandBuffer> _commandBuffers;

            std::vector<double> _slidingElapsedTime;
            int _slidingElapsedTimeIndex = 0;
            bool _pendingViewportResize = false;

            GLFWwindow * _window;

            Scene* _scene;
        };
    }
}

#endif