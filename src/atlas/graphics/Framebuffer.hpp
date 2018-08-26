#ifndef ATLAS_FRAMEBUFFER_HPP
#define ATLAS_FRAMEBUFFER_HPP

#include "AtlasGraphics.hpp"

namespace atlas
{
    namespace graphics
    {
        /**
        * @brief Encapsulates a Vulkan swapchain.
        */
        class Framebuffer
        {
        public:
            Framebuffer(GLFWwindow* window,
                vk::Device device,
                vk::PhysicalDevice gpu,
                vk::SurfaceKHR surface,
                uint32_t graphicsFamily, uint32_t presentFamily);

            ~Framebuffer();

            inline vk::RenderPass renderPass() const noexcept { return _renderPass; }
            inline vk::Framebuffer framebuffer(uint32_t i) const noexcept { return _framebuffers[i]; }

            uint32_t AcquireImage(vk::Semaphore semaphore);
            void PresentImage(uint32_t imageIndex, vk::Semaphore wait);

        private:
            void CreateSwapchain();
            void DestroySwapchain();
            void CreateImageViews();
            void DestroyImageViews();
            void CreateRenderPass();
            void DestroyRenderPass();
            void CreateFramebuffers();
            void DestroyFramebuffers();
            void CreateSemaphores();
            void DestroySemaphores();

            vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>&);
            vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>);
            vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR&, GLFWwindow*);

            std::shared_ptr<spdlog::logger> _log;

            GLFWwindow * _window;
            uint32_t _graphicsFamily;
            uint32_t _presentFamily;
            vk::PhysicalDevice _gpu;
            vk::Device _device;
            vk::Queue _presentQueue;
            vk::Queue _graphicsQueue;
            vk::SurfaceKHR _surface;
            vk::Format _format;
            vk::RenderPass _renderPass;
            vk::SwapchainKHR _swapchain;
            vk::Extent2D _extent;
            std::vector<vk::Image> _images;
            std::vector<vk::ImageView> _views;
            std::vector<vk::Framebuffer> _framebuffers;
            std::vector<vk::Semaphore> _semaphores;

            uint32_t _currentImage;
        };
    }
}

#endif // !ATLAS_FRAMEBUFFER_HPP
