#include "framebuffer.hpp"

using namespace atlas::renderer;

Framebuffer::Framebuffer(
    vk::RenderPass renderPass, vk::Format swapchainFormat,
    vk::Device device, vk::CommandBuffer cmd,
    vk::Extent2D extents, vk::Image swapchainImage,
    vk::ImageView depth) :
    _device(device),
    _cmdBuffer(cmd)
{
    _renderComplete = device.createSemaphore(vk::SemaphoreCreateInfo());
    _ownershipTransfer = device.createSemaphore(vk::SemaphoreCreateInfo());

    _swapchainImageView = device.createImageView(vk::ImageViewCreateInfo()
        .setImage(swapchainImage)
        .setViewType(vk::ImageViewType::e2D)
        .setFormat(swapchainFormat)
        .setComponents(vk::ComponentMapping())
        .setSubresourceRange(
            vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1)));

    const std::array<vk::ImageView, 2> attachments = { _swapchainImageView, depth };

    _framebuffer = device.createFramebuffer(vk::FramebufferCreateInfo()
        .setLayers(1)
        .setRenderPass(renderPass)
        .setHeight(extents.height)
        .setWidth(extents.width)
        .setAttachmentCount((uint32_t)attachments.size())
        .setPAttachments(attachments.data()));
}

Framebuffer::~Framebuffer()
{
    _device.destroyImageView(_swapchainImageView);
    _device.destroySemaphore(_renderComplete);
    _device.destroySemaphore(_ownershipTransfer);
    _device.destroyFramebuffer(_framebuffer);
}
