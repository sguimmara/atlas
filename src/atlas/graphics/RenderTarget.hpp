#ifndef ATLAS_GRAPHICS_RENDERTARGET_HPP
#define ATLAS_GRAPHICS_RENDERTARGET_HPP

#include "AtlasGraphics.hpp"

namespace atlas
{
    namespace graphics
    {
        struct RenderTarget
        {
            vk::Device device;

            uint32_t index;
            vk::Image image;
            vk::ImageView view;
            vk::Framebuffer framebuffer;

            vk::Semaphore imageAcquired;
            vk::Semaphore renderComplete;
            vk::Semaphore imageOwnership;
        };
    }
}

#endif