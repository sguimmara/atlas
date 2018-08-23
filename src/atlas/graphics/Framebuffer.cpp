#include "Framebuffer.hpp"

namespace atlas
{
    namespace graphics
    {
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
    {
        if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
        {
            return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
        }

        for (const auto& availableFormat : availableFormats)
        {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes)
    {
        VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

        for (const auto& availablePresentMode : availablePresentModes)
        {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return availablePresentMode;
            }
            else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
            {
                bestMode = availablePresentMode;
            }
        }

        return bestMode;
    }

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return capabilities.currentExtent;
        }
        else
        {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);

            VkExtent2D actualExtent =
            {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

            return actualExtent;
        }
    }

    Framebuffer::Framebuffer(GLFWwindow* window, VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t graphicsFamily, uint32_t presentFamily) :
        device(device),
        physicalDevice(physicalDevice),
        window(window),
        surface(surface)
    {
        vkGetDeviceQueue(device, graphicsFamily, 0, &graphicsQueue);
        vkGetDeviceQueue(device, presentFamily, 0, &presentQueue);

        createSwapchain(graphicsFamily, presentFamily);
        createImageViews();
        createRenderPass();
        createFramebuffers();
    }

    Framebuffer::~Framebuffer()
    {
        destroyImageViews();
        destroyRenderPass();
        destroyFramebuffers();
        destroySwapchain();

        logger->debug("swapchain destroyed");
    }

    void Framebuffer::createSwapchain(uint32_t graphicsFamily, uint32_t presentFamily)
    {
        SwapChainSupportDetails swapChainSupport = SwapChainSupportDetails::querySwapChainSupport(physicalDevice, surface);

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent2D = chooseSwapExtent(swapChainSupport.capabilities, window);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
        {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        logger = spdlog::get("renderer");

        logger->debug("swap chain image count : {0:d}", imageCount);

        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;

        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent2D;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        uint32_t queueFamilyIndices[] = { graphicsFamily, presentFamily };

        if (graphicsFamily != presentFamily)
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;

        VK_CHECK_RESULT(vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain));
        VK_CHECK_RESULT(vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr));
        images.resize(imageCount);
        VK_CHECK_RESULT(vkGetSwapchainImagesKHR(device, swapChain, &imageCount, images.data()));

        format = surfaceFormat.format;
        extent = extent2D;

        logger->debug("created swap chain ({0}*{1})", extent2D.width, extent2D.height);
    }

    void Framebuffer::destroySwapchain()
    {
        vkDestroySwapchainKHR(device, swapChain, nullptr);
    }

    void Framebuffer::createImageViews()
    {
        views.resize(images.size());

        for (size_t i = 0; i < views.size(); ++i)
        {
            VkImageViewCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            info.image = images[i];
            info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            info.format = format;

            info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            info.subresourceRange.baseMipLevel = 0;
            info.subresourceRange.levelCount = 1;
            info.subresourceRange.baseArrayLayer = 0;
            info.subresourceRange.layerCount = 1;

            VK_CHECK_RESULT(vkCreateImageView(device, &info, nullptr, &views[i]));
        }
    }

    void Framebuffer::destroyImageViews()
    {
        for (auto view : views)
        {
            vkDestroyImageView(device, view, nullptr);
        }
    }

    void Framebuffer::createFramebuffers()
    {
        framebuffers.resize(views.size());

        for (size_t i = 0; i < views.size(); i++)
        {
            VkImageView attachments[] = {
                views[i]
            };

            VkFramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = extent.width;
            framebufferInfo.height = extent.height;
            framebufferInfo.layers = 1;

            VK_CHECK_RESULT(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffers[i]));
        }
    }

    void Framebuffer::destroyFramebuffers()
    {
        for (auto fb : framebuffers)
        {
            vkDestroyFramebuffer(device, fb, nullptr);
        }
    }

    void Framebuffer::createRenderPass()
    {
        VkAttachmentDescription colorAttachment = {};
        colorAttachment.format = format;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef = {};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        info.attachmentCount = 1;
        info.pAttachments = &colorAttachment;
        info.subpassCount = 1;
        info.pSubpasses = &subpass;
        info.dependencyCount = 1;
        info.pDependencies = &dependency;

        VK_CHECK_RESULT(vkCreateRenderPass(device, &info, nullptr, &renderPass));
        logger->debug("created render pass");
    }

    void Framebuffer::destroyRenderPass()
    {
        vkDestroyRenderPass(device, renderPass, nullptr);
    }

    size_t Framebuffer::size()
    {
        return images.size();
    }

    VkDevice Framebuffer::getDevice()
    {
        return device;
    }

    VkRenderPass Framebuffer::getRenderPass()
    {
        return renderPass;
    }

    VkExtent2D Framebuffer::getExtent()
    {
        return extent;
    }

    VkFramebuffer Framebuffer::getFramebuffer(size_t index)
    {
        return framebuffers[index];
    }

    uint32_t Framebuffer::acquire(VkSemaphore imageAvailable)
    {
        uint32_t imageIndex;
        VK_CHECK_RESULT(vkAcquireNextImageKHR(device, swapChain, timeout, imageAvailable, VK_NULL_HANDLE, &imageIndex));
        return imageIndex;
    }

    void Framebuffer::present(uint32_t imageIndex, VkSemaphore renderFinished)
    {
        VkSwapchainKHR swapChains[] = { swapChain };
        VkSemaphore signalSemaphores[] = { renderFinished };

        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;

        VK_CHECK_RESULT(vkQueuePresentKHR(presentQueue, &presentInfo));
        VK_CHECK_RESULT(vkQueueWaitIdle(presentQueue));
    }
}
}
