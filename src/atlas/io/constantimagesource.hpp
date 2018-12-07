#ifndef ATLAS_IO_CONSTANTIMAGESOURCE_HPP
#define ATLAS_IO_CONSTANTIMAGESOURCE_HPP

#include "common.hpp"
#include "imagesource.hpp"
#include "atlas/core/imageformat.hpp"

namespace atlas::io
{
    using namespace atlas::core;

    // A data source that always return the same image.
    class ConstantImageSource final : public ImageSource
    {
    public:
        ConstantImageSource(Region, std::shared_ptr<Image> image);

        std::future<Response<Image>> get(Request<Region>) override;

        Region region() const noexcept { return _region; }

    private:
        Region _region;
        std::shared_ptr<Image> _image;

    };
}

#endif // ATLAS_IO_CONSTANTIMAGESOURCE_HPP