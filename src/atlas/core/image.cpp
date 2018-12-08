#include "image.hpp"
#include <filesystem>
#include <stb/stb_image.h>

using namespace atlas::core;

Image::Image() :
    _width(0),
    _height(0),
    _format(ImageFormat::Unknown),
    _data(nullptr)
{}

Image::Image(std::string filename)
{
    if (!std::filesystem::exists(filename))
    {
        throw std::runtime_error("no such file: " + filename);
    }

    int width, height, channels;

    auto srcFormat = STBI_rgb_alpha;
    int bpp = 4;

    unsigned char* rgba = stbi_load(filename.c_str(), &width, &height, &channels, srcFormat);
    if (!rgba)
    {
        throw std::runtime_error("failed to load texture image: " + filename);
    }

    if (srcFormat == STBI_rgb_alpha)
    {
        _format = ImageFormat::RGBA32;
    }
    else if (srcFormat == STBI_rgb)
    {
        _format = ImageFormat::RGB24;
    }
    else
    {
        throw std::runtime_error("unrecognized image format");
    }

    _width = (size_t)width;
    _height = (size_t)height;
    _data = std::make_unique<std::vector<std::byte>>(_width * _height * bpp);
    memcpy(_data->data(), rgba, _data->size());
    stbi_image_free(rgba);
}

Image::Image(Image& other) :
    _width(other._width),
    _height(other._height),
    _format(other._format)
{
    _data = std::make_unique<std::vector<std::byte>>(other._data->size());
    memcpy((void*)_data->data(), other._data->data(), other._data->size());
}

Image::Image(Image&& other) :
    _width(other._width),
    _height(other._height),
    _format(other._format),
    _data(std::move(other._data))
{
    other._data = nullptr;
    other._format = ImageFormat::Unknown;
    other._width = 0;
    other._height = 0;
}

Image::Image(size_t width, size_t height, ImageFormat format) :
    _width(width),
    _height(height),
    _format(format)
{
    _data = std::make_unique<std::vector<std::byte>>(_width * _height * getBpp(format));
}
