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

        Region region() const noexcept override { return _region; }

    protected:
        Response<Image> readRegion(Request<Region>) override;

    private:
        Region _region;
        std::vector<std::unique_ptr<std::promise<Response<Image>>>> _promises;
    };
}

#endif // ATLAS_IO_DEBUGIMAGESOURCE_HPP  