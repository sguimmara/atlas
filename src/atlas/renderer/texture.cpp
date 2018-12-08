#include "texture.hpp"
#include "instance.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

using namespace atlas::renderer;
using namespace atlas::core;

vk::Sampler defaultSampler()
{
    auto const samplerInfo = vk::SamplerCreateInfo()
        .setMagFilter(vk::Filter::eLinear)
        .setMinFilter(vk::Filter::eLinear)
        .setAddressModeU(vk::SamplerAddressMode::eRepeat)
        .setAddressModeV(vk::SamplerAddressMode::eRepeat)
        .setAddressModeW(vk::SamplerAddressMode::eRepeat)
        .setAnisotropyEnable(VK_FALSE)
        .setMaxAnisotropy(16)
        .setBorderColor(vk::BorderColor::eIntOpaqueBlack)
        .setUnnormalizedCoordinates(VK_FALSE)
        .setCompareEnable(VK_FALSE)
        .setCompareOp(vk::CompareOp::eAlways)
        .setMipmapMode(vk::SamplerMipmapMode::eLinear)
        .setMipLodBias(0)
        .setMinLod(0)
        .setMaxLod(0);

    return Instance::device.createSampler(samplerInfo);
}

Texture::Texture() :
    _view(nullptr),
    _sampler(nullptr),
    _image(nullptr)
{}

Texture::Texture(const Image& image)
{
    vk::Format format;
    switch (image.format())
    {
    case ImageFormat::RGB24:
        format = vk::Format::eR8G8B8Unorm;
        break;
    case ImageFormat::RGBA32:
        format = vk::Format::eR8G8B8A8Unorm;
        break;
    default:
        throw std::runtime_error("unrecognized image format");
    }

    create((int)image.width(), (int)image.height(), format, (void*)image.data());
}

Texture::Texture(const std::string& filename)
{
    // TODO check if file exists

    int width, height, channels;

    auto srcFormat = STBI_rgb_alpha;

    unsigned char* rgba = stbi_load(filename.c_str(), &width, &height, &channels, srcFormat);
    if (!rgba)
    {
        throw std::runtime_error("failed to load texture image: " + filename);
    }

    auto const format = vk::Format::eR8G8B8A8Unorm;

    create(width, height, format, rgba);
}

Texture::Texture(uint32_t width, uint32_t height, void* rgba)
{
    auto const format = vk::Format::eR8G8B8A8Unorm;

    create(width, height, format, rgba);
}

Texture::Texture(Texture&& rhs) :
    _view(rhs._view),
    _sampler(rhs._sampler),
    _image(rhs._image)
{
    rhs._sampler = nullptr;
    rhs._image = nullptr;
    rhs._view = nullptr;
}

Texture::~Texture()
{
    if (_image)
    {
        Allocator::free(_image);
    }
    if (_sampler)
    {
        Instance::device.destroySampler(_sampler);
    }
    if (_view)
    {
        Instance::device.destroyImageView(_view);
    }
}

void Texture::create(int width, int height, const vk::Format &format, void* rgba)
{
    _image = Allocator::getImage(vk::Extent2D(width, height), format, vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst);

    int size = width * height * 4;

    auto const stagingBuffer = Allocator::getStagingBuffer(size);
    Allocator::write(stagingBuffer, rgba, size);
    Allocator::transition(_image, format, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
    Allocator::copyBufferToImage(stagingBuffer, _image, width, height);
    Allocator::transition(_image, format, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
    Allocator::free(stagingBuffer);

    _view = Instance::device.createImageView(vk::ImageViewCreateInfo()
        .setImage(_image)
        .setViewType(vk::ImageViewType::e2D)
        .setFormat(format)
        .setSubresourceRange(vk::ImageSubresourceRange()
            .setAspectMask(vk::ImageAspectFlagBits::eColor)
            .setBaseMipLevel(0)
            .setLevelCount(1)
            .setBaseArrayLayer(0)
            .setLayerCount(1)));

    _sampler = defaultSampler();
}
