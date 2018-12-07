#include "image.hpp"

using namespace atlas::core;

Image::Image() :
    _width(0),
    _height(0),
    _format(ImageFormat::Unknown),
    _data(nullptr)
{}

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
