#ifndef ATLAS_RENDERER_FRAMEBUFFER_HPP
#define ATLAS_RENDERER_FRAMEBUFFER_HPP

#include "common.hpp"

namespace atlas::renderer
{
    class Framebuffer
    {
    public:
        Framebuffer(vk::RenderPass, vk::Format, vk::Device, vk::CommandBuffer,
            vk::Extent2D, vk::Image, vk::ImageView depth);

        inline vk::Framebuffer framebuffer() const noexcept { return _framebuffer; }
        inline vk::CommandBuffer cmdBuffer() const noexcept { return _cmdBuffer; }
        inline vk::Semaphore renderComplete() const noexcept { return _renderComplete; }
        inline vk::Semaphore ownershipTransfer() const noexcept { return _ownershipTransfer; }

        ~Framebuffer();

    private:
        vk::Device _device;
        vk::CommandBuffer _cmdBuffer;
        vk::Framebuffer _framebuffer;
        vk::ImageView _swapchainImageView;
        vk::Semaphore _renderComplete;
        vk::Semaphore _ownershipTransfer;
    };
}

#endif // ATLAS_RENDERER_FRAMEBUFFER_HPP