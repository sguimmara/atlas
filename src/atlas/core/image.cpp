#include "image.hpp"
#include <stb/stb_image.h>

using namespace atlas::core;

static size_t getBpp(ImageFormat format)
{
    switch (format)
    {
    case atlas::core::RGB24: return 3;
    case atlas::core::RGBA32: return 4;
    default: throw std::runtime_error("unimplemented image format");
    }
}

Image::Image() :
    _width(0),
    _height(0),
    _format(ImageFormat::Unknown),
    _data(nullptr)
{}

Image::Image(std::string filename)
{
    // TODO check if file exists

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
    _data = std::make_unique<std::vector<char>>(_width * _height * bpp);
    memcpy(_data->data(), rgba, _data->size());
    stbi_image_free(rgba);
}

Image::Image(Image& other) :
    _width(other._width),
    _height(other._height),
    _format(other._format)
{
    _data = std::make_unique<std::vector<char>>(other._data->size());
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

Image::Image(pixels width, pixels height, ImageFormat format, std::vector<char> data) :
    _width(width),
    _height(height),
    _format(format),
    _data(std::make_unique<std::vector<char>>(data))
{}

Image::Image(pixels width, pixels height, ImageFormat format) :
    _width(width),
    _height(height),
    _format(format)
{
    _data = std::make_unique<std::vector<char>>(_width * _height * getBpp(format));
}

std::shared_ptr<Image> Image::subImage(const Rect& rect) const
{
    assert(rect.x + rect.width <= _width);
    assert(rect.y + rect.height <= _height);

    // TODO handle BPP
    std::vector<char> sub(rect.width * rect.height * 4);

    size_t start = rect.x * 4;
    size_t end = start + rect.width * 4;
    size_t row = _width * 4;

    size_t k = 0;
    for (size_t j = rect.y; j < rect.y + rect.height; j++)
    {
        for (size_t i = start; i < end; i += 4)
        {
            sub[k++] = (*_data)[i + 0 + j * row];
            sub[k++] = (*_data)[i + 1 + j * row];
            sub[k++] = (*_data)[i + 2 + j * row];
            sub[k++] = (*_data)[i + 3 + j * row];
        }
    }

    return std::make_shared<Image>(rect.width, rect.height, ImageFormat::RGBA32, sub);
}
