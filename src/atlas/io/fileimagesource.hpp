#ifndef ATLAS_IO_FILEIMAGESOURCE_HPP
#define ATLAS_IO_FILEIMAGESOURCE_HPP

#include "common.hpp"
#include "imagesource.hpp"
#include "atlas/core/image.hpp"
#include "atlas/core/imageformat.hpp"

namespace atlas::io
{
    using namespace atlas::core;

    // A data source that reads from a single, non georeferenced file.
    // The file must be in the equirectangular projection (EPSG:4326).
    class FileImageSource final : public ImageSource
    {
    public:
        FileImageSource(Region, const std::string filename);

        std::future<Response<Image>> get(Request<Region>) override;

        inline std::string filename() const noexcept { return _filename; }

        Region region() const noexcept override { return _region; }

    private:
        Region _region;
        std::string _filename;
        std::shared_ptr<Image> _image;
        std::vector<std::unique_ptr<std::promise<Response<Image>>>> _promises;
        
        Response<Image> readRegion(Request<Region> request);
    };
}

#endif // ATLAS_IO_FILEIMAGESOURCE_HPP