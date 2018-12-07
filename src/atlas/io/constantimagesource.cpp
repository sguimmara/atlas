#include "constantimagesource.hpp"

using namespace atlas::io;

ConstantImageSource::ConstantImageSource(Region region, std::shared_ptr<Image> image) :
    _region(region),
    _image(image)
{}

std::future<Response<Image>> ConstantImageSource::get(Request<Region> request)
{
    return std::future<Response<Image>>();
}
