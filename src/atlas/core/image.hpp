#ifndef ATLAS_CORE_IMAGE_HPP
#define ATLAS_CORE_IMAGE_HPP

#include "common.hpp"
#include "rect.hpp"
#include "imageformat.hpp"

namespace atlas::core
{
    class Image
    {
    public:
        Image();
        Image(std::string filename);
        Image(Image&);
        Image(Image&&);
        Image(size_t width, size_t height, ImageFormat format, std::vector<char> data);
        Image(size_t width, size_t height, ImageFormat format);

        // returns the width in pixels of the image.
        inline size_t width() const noexcept { return _width; }

        // returns the height in pixels of the image.
        inline size_t height() const noexcept { return _height; }

        inline ImageFormat format() const noexcept { return _format; }

        // return an image
        std::shared_ptr<Image> subImage(const Rect&) const;

        // returns the size in bytes of the image buffer.
        inline size_t size() const noexcept { return _data->size(); }
        inline const char* data() const noexcept { return _data.get()->data(); }
    private:
        size_t _width;
        size_t _height;
        ImageFormat _format;
        std::unique_ptr<std::vector<char>> _data;
    };
}

#endif // ATLAS_CORE_IMAGE_HPP