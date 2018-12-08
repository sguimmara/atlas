#include "fileimagesource.hpp"

using namespace atlas::io;
using namespace atlas::core;

FileImageSource::FileImageSource(Region region, const std::string filename) :
    _region(region),
    _filename(filename),
    _image(std::make_unique<Image>(filename))
{}

std::future<Response<Image>> FileImageSource::get(Request<Region> request)
{
    auto promise = std::promise<Response<Image>>();
    auto producer = std::thread([&]
    {
        auto response = Response<Image>(_image, request.userData());

        promise.set_value(response);
    });

    // TODO: make it async
    producer.join();

    return promise.get_future();
}
