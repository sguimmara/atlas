#include "debugimagesource.hpp"
#include "atlas/core/rect.hpp"
#include "CImg/CImg.h"

using namespace atlas::io;
using namespace atlas::core;

using namespace cimg_library;

DebugImageSource::DebugImageSource(Region region) :
    _region(region)
{
}

Response<Image> DebugImageSource::readRegion(Request<Region> request)
{
    CImg<unsigned char> result(256, 256, 1, 4);
    result.fill(255, 0, 0, 255);
    auto data = result.data();
    auto size = result.size() * sizeof(unsigned char);
    std::vector<char> buffer;
    buffer.resize(size);
    for (size_t i = 0; i < size; i++)
    {
        buffer[i] = data[i];
    }
    auto image = std::make_shared<Image>(256, 256, ImageFormat::RGBA32, buffer);

    return Response<Image>(image, request.userData());
}

std::future<Response<Image>> DebugImageSource::get(Request<Region> request)
{
    std::future<Response<Image>> result(std::async(
        std::launch::async,
        [request, this]() { return readRegion(request); }));

    return result;
}
