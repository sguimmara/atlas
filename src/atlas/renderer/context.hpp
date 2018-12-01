#ifndef ATLAS_RENDERER_CONTEXT_HPP
#define ATLAS_RENDERER_CONTEXT_HPP

#include "common.hpp"
#include "framebuffer.hpp"

namespace atlas::renderer
{
    class Context
    {
    public:
        Context(GLFWwindow*, vk::SurfaceKHR, vk::SurfaceFormatKHR, vk::Device, vk::PhysicalDevice);
        ~Context();

        // the framebuffer size
        inline const vk::Extent2D size() const noexcept { return _size; }
        inline const vk::Viewport viewport() const noexcept { return _viewport; }

        void beginFrame();
        void endFrame();

    private:
        vk::SurfaceKHR _surface;
        GLFWwindow* _window;
        std::shared_ptr<spdlog::logger> _log;
        vk::Extent2D _size;
        vk::Viewport _viewport;
        vk::Device _device;
        vk::PhysicalDevice _physicalDevice;
        vk::SwapchainKHR _swapchain;
        std::vector<Framebuffer> _framebuffers;
        vk::CommandPool _commandPool;
        std::vector<vk::CommandBuffer> _commandBuffers;
        vk::Image _depthImage;
        vk::ImageView _depthAttachmentView;
        vk::Semaphore _imageAcquired;
        uint32_t _currentSwapchainImage;
    };
}

#endif // ATLAS_RENDERER_CONTEXT_HPP