#ifndef ATLAS_RENDERER_CONTEXT_HPP
#define ATLAS_RENDERER_CONTEXT_HPP

#include "common.hpp"
#include "framebuffer.hpp"
#include "mesh.hpp"
#include "material.hpp"
#include "global_properties.hpp"

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
        void setViewport(vk::Viewport);
        void bind(Pipeline*);
        void draw(vk::DescriptorSet globals, vk::DescriptorSet instanceSet, vk::DescriptorSet materialSet, const Mesh & mesh);
        void endFrame();

    private:
        // global context
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

        // frame context
        uint32_t _currentSwapchainImage;
        Pipeline* _currentPipeline;
        vk::PipelineLayout _currentPipelineLayout;
    };
}

#endif // ATLAS_RENDERER_CONTEXT_HPP