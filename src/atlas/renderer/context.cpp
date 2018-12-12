#include "context.hpp"
#include "allocator.hpp"
#include "instance.hpp"

using namespace atlas::renderer;

vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> modes)
{
    if (std::find(modes.begin(), modes.end(), vk::PresentModeKHR::eFifo) != modes.end())
    {
        return vk::PresentModeKHR::eFifo;
    }
    if (std::find(modes.begin(), modes.end(), vk::PresentModeKHR::eFifoRelaxed) != modes.end())
    {
        return vk::PresentModeKHR::eFifoRelaxed;
    }

    return modes[0];
}

vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, GLFWwindow* window)
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

uint32_t chooseImageCount(vk::SurfaceCapabilitiesKHR capabilities)
{
    uint32_t imageCount = capabilities.minImageCount + 1;

    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
    {
        imageCount = capabilities.maxImageCount;
    }

    return imageCount;
}

Context::Context(GLFWwindow * window, vk::SurfaceKHR surface,
    vk::SurfaceFormatKHR surfaceFormat,
    vk::Device device, vk::PhysicalDevice physicalDevice) :
    _surface(surface),
    _device(device),
    _physicalDevice(physicalDevice)
{
    _log = spdlog::get("context");
    if (!_log)
    {
        _log = spdlog::stdout_color_mt("context");
    }

    _imageAcquired = device.createSemaphore(vk::SemaphoreCreateInfo());

    _commandPool = device.createCommandPool(vk::CommandPoolCreateInfo()
        .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer));

    auto const surfaceCapabilities = _physicalDevice.getSurfaceCapabilitiesKHR(surface);
    _size = chooseSwapExtent(surfaceCapabilities, window);
    _viewport = vk::Viewport(0, 0, (float)_size.width, (float)_size.height);
    _viewport.setMaxDepth(1);
    _viewport.setMinDepth(0);
    auto const presentMode = chooseSwapPresentMode(_physicalDevice.getSurfacePresentModesKHR(surface));
    auto const imageCount = chooseImageCount(surfaceCapabilities);

    // swapchain creation
    _swapchain = device.createSwapchainKHR(vk::SwapchainCreateInfoKHR()
        .setSurface(surface)
        .setImageExtent(_size)
        .setMinImageCount(imageCount)
        .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
        .setPresentMode(presentMode)
        .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
        .setClipped(VK_TRUE)
        .setImageArrayLayers(1)
        .setPreTransform(surfaceCapabilities.currentTransform)
        .setImageFormat(surfaceFormat.format)
        .setImageColorSpace(surfaceFormat.colorSpace));

    _log->trace("swapchain created. format: {0}/{1}/{2}, {3}*{4}",
        vk::to_string(surfaceFormat.format),
        vk::to_string(surfaceFormat.colorSpace),
        vk::to_string(presentMode),
        _size.width,
        _size.height);

    auto const swapchainImages = device.getSwapchainImagesKHR(_swapchain);
    _commandBuffers = device.allocateCommandBuffers(vk::CommandBufferAllocateInfo()
        .setCommandBufferCount((uint32_t)swapchainImages.size())
        .setLevel(vk::CommandBufferLevel::ePrimary)
        .setCommandPool(_commandPool));

    _depthImage = Allocator::getImage(_size, vk::Format::eD24UnormS8Uint,
        vk::ImageUsageFlagBits::eDepthStencilAttachment);

    _depthAttachmentView = device.createImageView(vk::ImageViewCreateInfo()
        .setImage(_depthImage)
        .setFormat(vk::Format::eD24UnormS8Uint)
        .setViewType(vk::ImageViewType::e2D)
        .setSubresourceRange(
            vk::ImageSubresourceRange()
            .setAspectMask(vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil)
            .setBaseMipLevel(0)
            .setLevelCount(1)
            .setBaseArrayLayer(0)
            .setLayerCount(1)));

    _framebuffers.reserve(3);

    for (size_t i = 0; i < swapchainImages.size(); i++)
    {
        _framebuffers.emplace_back(
            Instance::renderPass, surfaceFormat.format, _device,
            _commandBuffers[i], _size, swapchainImages[i],
            _depthAttachmentView);
    }
}

Context::~Context()
{
    Allocator::free(_depthImage);
    _device.destroySemaphore(_imageAcquired);
    _device.destroyImageView(_depthAttachmentView);
    _device.freeCommandBuffers(_commandPool, (uint32_t)_commandBuffers.size(), _commandBuffers.data());
    _device.destroyCommandPool(_commandPool);
    _log->trace("renderpass destroyed");
    _device.destroySwapchainKHR(_swapchain);
    _log->trace("swapchain destroyed");
    _log->trace("context destroyed");
    _log = nullptr;
}

void Context::beginFrame()
{
    _currentPipeline = nullptr;
    auto const img = _device.acquireNextImageKHR(_swapchain, 100, _imageAcquired, vk::Fence());
    check(img.result);
    _currentSwapchainImage = img.value;

    vk::ClearValue const clearValues[2] = {
         vk::ClearColorValue(std::array<float, 4>({0.0f, 0.0f, 0.0f, 1})),
         vk::ClearDepthStencilValue(1.0f, 0u) };

    auto const scissor = vk::Rect2D({ 0, 0 }, _size);

    Framebuffer& fb = _framebuffers[_currentSwapchainImage];
    auto framebuffer = fb.framebuffer();
    auto buffer = fb.cmdBuffer();

    auto const begin = vk::CommandBufferBeginInfo()
        .setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse);

    check(buffer.begin(&begin));
    {
        auto const renderPassInfo = vk::RenderPassBeginInfo()
            .setRenderPass(Instance::renderPass)
            .setFramebuffer(framebuffer)
            .setRenderArea(vk::Rect2D(vk::Offset2D(0, 0), _size))
            .setClearValueCount(2)
            .setPClearValues(clearValues);

        buffer.setViewport(0, 1, &_viewport);
        buffer.setScissor(0, 1, &scissor);
        buffer.beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);
    }
}

void Context::setViewport(vk::Viewport viewport)
{
    viewport.setMaxDepth(1);
    viewport.setMinDepth(0);
    auto const cmd = _framebuffers[_currentSwapchainImage].cmdBuffer();
    cmd.setViewport(0, 1, &viewport);
}

void Context::bind(Pipeline* pipeline)
{
    if (_currentPipeline != pipeline)
    {
        auto const cmd = _framebuffers[_currentSwapchainImage].cmdBuffer();
        cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline->vkPipeline());
        _currentPipeline = pipeline;
        _currentPipelineLayout = pipeline->layout();
    }
}

void Context::draw(vk::DescriptorSet globalSet, vk::DescriptorSet instanceSet, vk::DescriptorSet materialSet, const Mesh& mesh)
{
    auto const cmd = _framebuffers[_currentSwapchainImage].cmdBuffer();

    std::array<vk::DescriptorSet, 3> sets = { globalSet, instanceSet, materialSet };
    //std::array<vk::DescriptorSet, 2> sets = { _globalPropertySet, instanceSet };

    cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, _currentPipelineLayout, 0, (uint32_t)sets.size(), sets.data(), 0, nullptr);

    auto const vOffset = mesh.vertexOffset();

    auto const buffer = mesh.buffer();
    cmd.bindVertexBuffers(0, 1, &buffer, &vOffset);
    if (mesh.indexCount() > 0)
    {
        cmd.bindIndexBuffer(mesh.buffer(), mesh.indexOffset(), vk::IndexType::eUint16);
        cmd.drawIndexed(mesh.indexCount(), 1, 0, 0, 0);
    }
    else
    {
        cmd.draw(mesh.vertexCount(), 1, 0, 0);
    }
}

void Context::endFrame()
{
    Framebuffer& fb = _framebuffers[_currentSwapchainImage];
    auto buffer = fb.cmdBuffer();

    buffer.endRenderPass();
    buffer.end();

    auto const renderComplete = fb.renderComplete();
    auto const ownershipTransfer = fb.ownershipTransfer();

    vk::PipelineStageFlags const pipeStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    auto const info = vk::SubmitInfo()
        .setWaitSemaphoreCount(1)
        .setPWaitSemaphores(&_imageAcquired)
        .setSignalSemaphoreCount(1)
        .setPSignalSemaphores(&renderComplete)
        .setCommandBufferCount(1)
        .setPCommandBuffers(&buffer)
        .setPWaitDstStageMask(&pipeStage);

    check(Instance::graphicsQueue.submit(1, &info, vk::Fence()));

    bool separateQueues = Instance::presentQueue != Instance::graphicsQueue;
    if (separateQueues)
    {
        auto const presentInfo = vk::SubmitInfo()
            .setPWaitDstStageMask(&pipeStage)
            .setWaitSemaphoreCount(1)
            .setPWaitSemaphores(&renderComplete)
            .setCommandBufferCount(0)
            .setSignalSemaphoreCount(1)
            .setPSignalSemaphores(&ownershipTransfer);

        check(Instance::presentQueue.submit(1, &presentInfo, vk::Fence()));
    }

    _device.waitIdle();

    auto const presentInfo = vk::PresentInfoKHR()
        .setWaitSemaphoreCount(1)
        .setPWaitSemaphores(separateQueues ? &ownershipTransfer : &renderComplete)
        .setSwapchainCount(1)
        .setPSwapchains(&_swapchain)
        .setPImageIndices(&_currentSwapchainImage);

    check(Instance::presentQueue.presentKHR(&presentInfo));

    Instance::presentQueue.waitIdle();
}
