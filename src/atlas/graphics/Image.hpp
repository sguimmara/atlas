#ifndef ATLAS_GRAPHICS_IMAGES
#define ATLAS_GRAPHICS_IMAGES

#include "AtlasGraphics.hpp"

namespace atlas
{
    namespace graphics
    {
        class Renderer;

        struct Image
        {
            uint32_t width;
            uint32_t height;
            vk::Image image;
            vk::ImageView view;
            vk::Sampler sampler;
            vk::DeviceMemory memory;

            void Destroy(vk::Device device);

            static Image FromFile(Renderer* renderer, std::string path);
            static Image FromRGBA(Renderer* renderer, void* rgba32, uint32_t width, uint32_t height);
        };
    }
}

#endif