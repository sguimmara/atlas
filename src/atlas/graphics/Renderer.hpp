#ifndef ATLAS_GRAPHICS_RENDERER_HPP
#define ATLAS_GRAPHICS_RENDERER_HPP

#include "AtlasGraphics.hpp"
#include "Framebuffer.hpp"

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
            inline Framebuffer* framebuffer() const noexcept { return _framebuffer; }

            void Run();

        private:
            struct FrameInfo
            {
                uint32_t index;
                vk::Semaphore imageAcquired;
                vk::Semaphore renderComplete;
                vk::Semaphore imageOwnership;

                void destroy()
                {
                    _device.destroySemaphore(imageAcquired);
                    _device.destroySemaphore(renderComplete);
                    _device.destroySemaphore(imageOwnership);
                }

                FrameInfo(vk::Device device) : _device(device)
                {
                    auto const info = vk::SemaphoreCreateInfo();

                    device.createSemaphore(&info, nullptr, &imageAcquired);
                    device.createSemaphore(&info, nullptr, &renderComplete);
                    device.createSemaphore(&info, nullptr, &imageOwnership);
                }

            private:
                vk::Device _device;
            };

            void CreateInstance();
            void DestroyInstance();
            void SetupDebugCallbacks();
            void CreateSurface();
            void SelectGpu();
            bool CheckExtensionSupport(vk::PhysicalDevice gpu);
            void CreateDevice();
            void DestroyDevice();

            void CreateFramebuffer();
            void DestroyFrameBuffer();

            void CreateCommandPool();
            void DestroyCommandPool();
            void CreateCommandBuffers();
            void UpdateCommandBuffers();

            void CreateImages();
            void DestroyImages();

            void RenderFrame();
            void SubmitFrame(FrameInfo image, vk::CommandBuffer buffer);

            uint32_t _imageLag = 3;

            std::shared_ptr<spdlog::logger> _log;

            vk::Instance _instance;
            vk::PhysicalDevice _gpu;
            vk::Device _device;
            vk::SwapchainKHR _swapchain;
            vk::SurfaceKHR _surface;
            vk::CommandPool _commandPool;
            vk::Queue _presentQueue;
            vk::Queue _graphicsQueue;
            vk::Extent2D _extent;
            vk::Viewport _viewport;
            VkDebugReportCallbackEXT _debugCallback;

            Framebuffer * _framebuffer = nullptr;
            uint32_t _presentFamily;
            uint32_t _graphicsFamily;
            uint32_t _imageIndex;

            std::vector<const char*> _enabledExtensions;
            std::vector<FrameInfo> _images;
            std::vector<vk::CommandBuffer> _commandBuffers;

            Drawable* _drawable;

            GLFWwindow * _window;
        };
    }
}

#endif