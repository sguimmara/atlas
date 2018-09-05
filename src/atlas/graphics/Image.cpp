#include "Image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "Renderer.hpp"

namespace atlas
{
    namespace graphics
    {
        void Image::Destroy(vk::Device device)
        {
            width = 0;
            height = 0;
            device.destroySampler(sampler);
            device.destroyImageView(view);
            device.destroyImage(image);
            device.freeMemory(memory);
        }

        Image Image::FromFile(Renderer* renderer, std::string path)
        {
            int width, height, channels;
            const auto pixels = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
            if (!pixels)
            {
                throw std::runtime_error("failed to load texture image: " + path);
            }

            Image img = FromRGBA(renderer, pixels, width, height);
            stbi_image_free(pixels);
            return img;
        }

        Image Image::FromRGBA(Renderer* renderer, void* rgba32, uint32_t width, uint32_t height)
        {
            vk::Buffer stagingBuffer;
            vk::DeviceMemory stagingBufferMemory;

            vk::DeviceSize size = width * height * 4;

            renderer->CreateBuffer(size, rgba32, &stagingBuffer, &stagingBufferMemory, vk::BufferUsageFlagBits::eTransferSrc);

            vk::Image texture;
            vk::DeviceMemory textureMemory;

            auto const format = vk::Format::eR8G8B8A8Unorm;

            auto const imageInfo = vk::ImageCreateInfo()
                .setImageType(vk::ImageType::e2D)
                .setExtent(vk::Extent3D(vk::Extent2D(width, height), 1))
                .setMipLevels(1)
                .setArrayLayers(1)
                .setFormat(format)
                .setTiling(vk::ImageTiling::eOptimal)
                .setInitialLayout(vk::ImageLayout::eUndefined)
                .setUsage(vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled)
                .setSharingMode(vk::SharingMode::eExclusive)
                .setSamples(vk::SampleCountFlagBits::e1);

            CHECK_SUCCESS(renderer->device().createImage(&imageInfo, nullptr, &texture));

            vk::MemoryRequirements memReq = renderer->device().getImageMemoryRequirements(texture);

            auto const allocInfo = vk::MemoryAllocateInfo()
                .setAllocationSize(memReq.size)
                .setMemoryTypeIndex(renderer->GetMemoryIndex(memReq, vk::MemoryPropertyFlagBits::eDeviceLocal));

            CHECK_SUCCESS(renderer->device().allocateMemory(&allocInfo, nullptr, &textureMemory));

            renderer->device().bindImageMemory(texture, textureMemory, 0);

            renderer->TransitionImageLayout(texture, format, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
            renderer->CopyBufferToImage(width, height, stagingBuffer, texture);
            renderer->TransitionImageLayout(texture, format, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

            renderer->device().destroyBuffer(stagingBuffer);
            renderer->device().freeMemory(stagingBufferMemory);

            auto const viewInfo = vk::ImageViewCreateInfo()
                .setImage(texture)
                .setViewType(vk::ImageViewType::e2D)
                .setFormat(format)
                .setSubresourceRange(vk::ImageSubresourceRange()
                    .setAspectMask(vk::ImageAspectFlagBits::eColor)
                    .setBaseMipLevel(0)
                    .setLevelCount(1)
                    .setBaseArrayLayer(0)
                    .setLayerCount(1));

            vk::ImageView view;
            CHECK_SUCCESS(renderer->device().createImageView(&viewInfo, nullptr, &view));

            auto const samplerInfo = vk::SamplerCreateInfo()
                .setMagFilter(vk::Filter::eLinear)
                .setMinFilter(vk::Filter::eLinear)
                .setAddressModeU(vk::SamplerAddressMode::eRepeat)
                .setAddressModeV(vk::SamplerAddressMode::eRepeat)
                .setAddressModeW(vk::SamplerAddressMode::eRepeat)
                .setAnisotropyEnable(VK_TRUE)
                .setMaxAnisotropy(16)
                .setBorderColor(vk::BorderColor::eIntOpaqueBlack)
                .setUnnormalizedCoordinates(VK_FALSE)
                .setCompareEnable(VK_FALSE)
                .setCompareOp(vk::CompareOp::eAlways)
                .setMipmapMode(vk::SamplerMipmapMode::eLinear)
                .setMipLodBias(0.0f)
                .setMinLod(0.0f)
                .setMaxLod(0.0f);

            vk::Sampler sampler;
            CHECK_SUCCESS(renderer->device().createSampler(&samplerInfo, nullptr, &sampler));

            // TODO
            return Image{ width, height, texture, view, sampler, textureMemory };
        }
    }
}