#include "Framebuffer.hpp"

namespace atlas
{
    namespace graphics
    {
        Framebuffer::Framebuffer(GLFWwindow * window,
            vk::Device device,
            vk::PhysicalDevice gpu,
            vk::SurfaceKHR surface,
            uint32_t graphicsFamily, uint32_t presentFamily) :
            _window(window),
            _device(device),
            _gpu(gpu),
            _surface(surface),
            _graphicsFamily(graphicsFamily),
            _presentFamily(presentFamily),
            _currentImage(0)
        {
            _device.getQueue(graphicsFamily, 0, &_graphicsQueue);
            _device.getQueue(presentFamily, 0, &_presentQueue);

            CreateSwapchain();
            CreateImageViews();
            CreateRenderPass();
            CreateFramebuffers();
            CreateSemaphores();
        }

        Framebuffer::~Framebuffer()
        {
            DestroySemaphores();
            DestroyFramebuffers();
            DestroyRenderPass();
            DestroyImageViews();
            DestroySwapchain();

            _log->debug("destroyed framebuffer");
        }

        uint32_t Framebuffer::AcquireImage(vk::Semaphore semaphore)
        {
            assert(_semaphores.size() == _views.size());
            assert(_images.size() == _views.size());
            assert(_framebuffers.size() == _views.size());

            uint32_t result;
            uint64_t timeout = 1000;
            //vk::Semaphore sem = _semaphores[_currentImage];
            //semaphore = sem;
            VERIFY(_device.acquireNextImageKHR(_swapchain, timeout, semaphore, vk::Fence(), &result) == vk::Result::eSuccess);
            ++_currentImage;
            _currentImage %= _views.size();
            return result;
        }

        void Framebuffer::PresentImage(uint32_t imageIndex, vk::Semaphore wait)
        {
            auto const info = vk::PresentInfoKHR()
                .setWaitSemaphoreCount(1)
                .setPWaitSemaphores(&wait)
                .setSwapchainCount(1)
                .setPSwapchains(&_swapchain)
                .setPImageIndices(&imageIndex);

            VERIFY(_presentQueue.presentKHR(&info) == vk::Result::eSuccess);
            _presentQueue.waitIdle();
        }

        vk::PresentModeKHR Framebuffer::ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR> availablePresentModes)
        {
            vk::PresentModeKHR bestMode = vk::PresentModeKHR::eFifo;

            for (const auto& availablePresentMode : availablePresentModes)
            {
                if (availablePresentMode == vk::PresentModeKHR::eMailbox)
                {
                    return availablePresentMode;
                }
                else if (availablePresentMode == vk::PresentModeKHR::eImmediate)
                {
                    bestMode = availablePresentMode;
                }
            }

            return bestMode;
        }

        vk::Extent2D Framebuffer::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, GLFWwindow* window)
        {
            if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
            {
                return capabilities.currentExtent;
            }
            else
            {
                int width, height;
                glfwGetFramebufferSize(window, &width, &height);

                vk::Extent2D actualExtent =
                {
                    static_cast<uint32_t>(width),
                    static_cast<uint32_t>(height)
                };

                actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
                actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

                return actualExtent;
            }
        }

        vk::SurfaceFormatKHR Framebuffer::ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
        {
            if (availableFormats.size() == 1 && availableFormats[0].format == vk::Format::eUndefined)
            {
                return { vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear };
            }

            for (const auto& availableFormat : availableFormats)
            {
                if (availableFormat.format == vk::Format::eB8G8R8A8Unorm && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
                {
                    return availableFormat;
                }
            }

            return availableFormats[0];
        }

        void Framebuffer::CreateSwapchain()
        {
            SwapChainSupportDetails swapChainSupport = SwapChainSupportDetails::QuerySwapChainSupport(_gpu, _surface);

            vk::SurfaceFormatKHR format = ChooseSwapSurfaceFormat(swapChainSupport.formats);
            vk::PresentModeKHR mode = ChooseSwapPresentMode(swapChainSupport.presentModes);
            vk::Extent2D extent = ChooseSwapExtent(swapChainSupport.capabilities, _window);
            uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
            if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
            {
                imageCount = swapChainSupport.capabilities.maxImageCount;
            }

            _log = spdlog::get("renderer");
            _log->debug("swapchain image count :  {0:d}", imageCount);

            auto info = vk::SwapchainCreateInfoKHR()
                .setSurface(_surface)
                .setMinImageCount(imageCount)
                .setImageFormat(format.format)
                .setImageColorSpace(format.colorSpace)
                .setImageExtent(extent)
                .setImageArrayLayers(1)
                .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
                .setPreTransform(swapChainSupport.capabilities.currentTransform)
                .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
                .setPresentMode(mode)
                .setClipped(VK_TRUE);

            uint32_t queueFamilyIndices[] = { _graphicsFamily, _presentFamily };

            if (_graphicsFamily != _presentFamily)
            {
                info.imageSharingMode = vk::SharingMode::eConcurrent;
                info.queueFamilyIndexCount = 2;
                info.pQueueFamilyIndices = queueFamilyIndices;
            }
            else
            {
                info.imageSharingMode = vk::SharingMode::eExclusive;
            }

            _device.createSwapchainKHR(&info, nullptr, &_swapchain);
            _device.getSwapchainImagesKHR(_swapchain, &imageCount, nullptr);
            _images.resize(imageCount);
            _device.getSwapchainImagesKHR(_swapchain, &imageCount, _images.data());

            _format = format.format;
            _extent = extent;

            _log->debug("created swapchain");
        }

        void Framebuffer::DestroySwapchain()
        {
            _device.destroySwapchainKHR(_swapchain, nullptr);
            _log->debug("destroyed swapchain");
        }

        void Framebuffer::CreateImageViews()
        {
            _views.resize(_images.size());
            vk::Result result;

            for (size_t i = 0; i < _views.size(); i++)
            {
                auto info = vk::ImageViewCreateInfo()
                    .setImage(_images[i])
                    .setViewType(vk::ImageViewType::e2D)
                    .setFormat(_format)
                    .setComponents(vk::ComponentMapping())
                    .setSubresourceRange(
                        vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));

                result = _device.createImageView(&info, nullptr, &_views[i]);
                CHECK_SUCCESS(result);
            }
        }

        void Framebuffer::DestroyImageViews()
        {
            for (auto view : _views)
            {
                _device.destroyImageView(view, nullptr);
            }
            _views.clear();
        }

        void Framebuffer::CreateRenderPass()
        {
            auto const attachment = vk::AttachmentDescription()
                .setFormat(_format)
                .setSamples(vk::SampleCountFlagBits::e1)
                .setLoadOp(vk::AttachmentLoadOp::eClear)
                .setStoreOp(vk::AttachmentStoreOp::eStore)
                .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
                .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
                .setInitialLayout(vk::ImageLayout::eUndefined)
                .setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

            auto const attachmentRef = vk::AttachmentReference()
                .setAttachment(0)
                .setLayout(vk::ImageLayout::eColorAttachmentOptimal);

            auto const subpass = vk::SubpassDescription()
                .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
                .setColorAttachmentCount(1)
                .setPColorAttachments(&attachmentRef);

            auto const dependency = vk::SubpassDependency()
                .setSrcSubpass(VK_SUBPASS_EXTERNAL)
                .setDstSubpass(0)
                .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
                .setSrcAccessMask((vk::AccessFlagBits)0)
                .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
                .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite);

            auto const info = vk::RenderPassCreateInfo()
                .setAttachmentCount(1)
                .setPAttachments(&attachment)
                .setSubpassCount(1)
                .setPSubpasses(&subpass)
                .setDependencyCount(1)
                .setPDependencies(&dependency);

            auto const result = _device.createRenderPass(&info, nullptr, &_renderPass);
            CHECK_SUCCESS(result);
            _log->debug("created render pass");
        }

        void Framebuffer::DestroyRenderPass()
        {
            _device.destroyRenderPass(_renderPass, nullptr);
            _log->debug("destroyed render pass");
        }

        void Framebuffer::CreateFramebuffers()
        {
            _framebuffers.resize(_views.size());
            vk::Result result;

            for (size_t i = 0; i < _views.size(); i++)
            {
                vk::ImageView attachments[] = { _views[i] };
                auto const info = vk::FramebufferCreateInfo()
                    .setRenderPass(_renderPass)
                    .setAttachmentCount(1)
                    .setPAttachments(attachments)
                    .setWidth(_extent.width)
                    .setHeight(_extent.height)
                    .setLayers(1);

                result = _device.createFramebuffer(&info, nullptr, &_framebuffers[i]);
                CHECK_SUCCESS(result);
            }
        }

        void Framebuffer::DestroyFramebuffers()
        {
            for (auto fb : _framebuffers)
            {
                _device.destroyFramebuffer(fb, nullptr);
            }

            _framebuffers.clear();
        }

        void Framebuffer::CreateSemaphores()
        {
            _semaphores.resize(_views.size());
            vk::Result result;

            for (size_t i = 0; i < _images.size(); i++)
            {
                auto const info = vk::SemaphoreCreateInfo();
                result = _device.createSemaphore(&info, nullptr, &_semaphores[i]);
                CHECK_SUCCESS(result);
            }
        }

        void Framebuffer::DestroySemaphores()
        {
            for (auto sem : _semaphores)
            {
                _device.destroySemaphore(sem);
            }
            _semaphores.clear();
        }
    }
}
