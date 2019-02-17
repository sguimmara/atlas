#pragma once
#ifndef ATLAS_IO_DEBUGIMAGESOURCE_HPP
#define ATLAS_IO_DEBUGIMAGESOURCE_HPP

#include "common.hpp"
#include "imagesource.hpp"
#include "atlas/core/image.hpp"
#include "atlas/core/imageformat.hpp"

namespace atlas::io
{
    using namespace atlas::core;

    // A data source that returns solid colored images.
    class DebugImageSource final : public ImageSource
    {
    public:
        DebugImageSource(Region);

        std::future<Response<Image>> get(Request<Region>) override;

        Region region() const noexcept override { return _region; }

    private:
        Region _region;
        std::vector<std::unique_ptr<std::promise<Response<Image>>>> _promises;

        Response<Image> readRegion(Request<Region> request);
    };
}

#endif // ATLAS_IO_DEBUGIMAGESOURCE_HPP  