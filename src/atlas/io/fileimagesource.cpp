#include "fileimagesource.hpp"
#include "atlas/core/rect.hpp"

using namespace atlas::io;
using namespace atlas::core;

FileImageSource::FileImageSource(Region region, const std::string filename) :
    _region(region),
    _filename(filename),
    _image(std::make_unique<Image>(filename))
{}

Response<Image> FileImageSource::readRegion(Request<Region> request)
{
    // TODO remove
    //std::this_thread::sleep_for(std::chrono::milliseconds(500));

    auto region = request.key();

    Rect rect;

    double x = ((region.west() + PI) / TWO_PI) * _image->width();
    double y = ((region.south() + HALF_PI) / PI) * _image->height();
    double w = (region.width() / TWO_PI) * _image->width();
    double h = (region.height() / PI) * _image->height();

    rect.x = (uint32_t)x;
    rect.y = (uint32_t)y;
    rect.width = (uint32_t)w;
    rect.height = (uint32_t)h;

    return Response<Image>(_image->subImage(rect), request.userData());
}

std::future<Response<Image>> FileImageSource::get(Request<Region> request)
{
    std::future<Response<Image>> result(std::async(
        std::launch::async,
        [request, this]() { return readRegion(request); }));

    return result;
}
