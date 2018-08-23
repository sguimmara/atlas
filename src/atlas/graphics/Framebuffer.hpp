#ifndef ATLAS_FRAMEBUFFER_HPP
#define ATLAS_FRAMEBUFFER_HPP

#include "AtlasGraphics.hpp"

namespace atlas
{
    namespace graphics
    {
        /**
        * @brief Encapsulates a Vulkan framebuffer.
        */
        class Framebuffer
        {
        public:
            Framebuffer(GLFWwindow*, VkDevice, VkPhysicalDevice, VkSurfaceKHR, uint32_t, uint32_t);
            ~Framebuffer();

            size_t size();
            VkDevice getDevice();
            VkRenderPass getRenderPass();
            VkExtent2D getExtent();
            VkFramebuffer getFramebuffer(size_t index);

            uint32_t acquire(VkSemaphore imageAvailable);
            void present(uint32_t imageIndex, VkSemaphore renderFinished);

        private:
            std::shared_ptr<spdlog::logger> logger;

            GLFWwindow* window;
            VkSurfaceKHR surface;
            VkSwapchainKHR swapChain;
            VkFormat format;
            VkExtent2D extent;
            VkPhysicalDevice physicalDevice;
            VkDevice device;
            VkRenderPass renderPass;
            VkQueue graphicsQueue;
            VkQueue presentQueue;

            std::vector<VkImage> images;
            std::vector<VkImageView> views;
            std::vector<VkFramebuffer> framebuffers;

            const uint64_t timeout = std::numeric_limits<uint64_t>::max();

            void createSwapchain(uint32_t graphicsFamily, uint32_t presentFamily);
            void destroySwapchain();
            void createImageViews();
            void destroyImageViews();
            void createFramebuffers();
            void destroyFramebuffers();
            void createRenderPass();
            void destroyRenderPass();
        };
    }
}

#endif // !ATLAS_FRAMEBUFFER_HPP
