#include "imagesource.hpp"

using namespace atlas::io;

std::future<Response<Image>> ImageSource::get(Request<Region> request)
{
    std::future<Response<Image>> result(std::async(
        std::launch::async,
        [request, this]() { return readRegion(request); }));

    return result;
}
