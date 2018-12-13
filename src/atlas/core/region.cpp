#include "region.hpp"

#include <stdexcept>

using namespace atlas::core;

Region::Region()
{}

Region::Region(Cartographic min, Cartographic max) :
    _min(min),
    _max(max)
{
    if (min.longitude > max.longitude || min.latitude > max.latitude)
    {
        throw std::invalid_argument("invalid min/max values : max is lower than min");
    }
}

Region::Region(Cartographic min, double width, double height) :
    _min(min),
    _max(Cartographic(min.latitude + height, min.longitude + width))
{
    if (width < 0 || height < 0)
    {
        throw std::invalid_argument("width and height should be positive");
    }
}

Region Region::combine(Region a, Region b)
{
    Region result = Region(a);

    result.expand(b);

    return result;
}

void Region::expand(Region& other)
{
    double minlon = std::fmin(other._min.longitude, _min.longitude);
    double minlat = std::fmin(other._min.latitude, _min.latitude);
    double minhgt = std::fmin(other._min.height, _min.height);

    double maxlon = std::fmax(other._max.longitude, _max.longitude);
    double maxlat = std::fmax(other._max.latitude, _max.latitude);
    double maxhgt = std::fmax(other._max.height, _max.height);

    _min = Cartographic(minlat, minlon, minhgt);
    _max = Cartographic(maxlat, maxlon, maxhgt);
}

bool Region::intersects(Region& other)
{
    if (!(_min.longitude > other._max.longitude ||
        _max.longitude < other._min.longitude ||
        _max.latitude > other._min.latitude ||
        _min.latitude > other._max.latitude))
    {
        return true;
    }

    return false;
}

bool Region::contains(Cartographic point)
{
    return _min.longitude <= point.longitude
        && _min.latitude <= point.latitude
        && _max.longitude >= point.longitude
        && _max.latitude >= point.latitude;
}

std::vector<Region> Region::subdivide(size_t xCells, size_t yCell) const noexcept
{
    auto result = std::vector<Region>();
    result.reserve(xCells * yCell);

    double xStep = width() / xCells;
    double yStep = height() / yCell;

    double x, y;

    for (size_t i = 0; i < xCells; i++)
    {
        x = west() + xStep * i;

        for (size_t j = 0; j < yCell; j++)
        {
            y = north() - (yStep * j);

            result.emplace_back(Cartographic(y - yStep, x), xStep, yStep);
        }
    }

    return result;
}

const Cartographic Region::topLeft() const noexcept
{
    return Cartographic(_max.latitude, _min.longitude, _min.height);
}

const Cartographic Region::topRight() const noexcept
{
    return Cartographic(_max.latitude, _max.longitude, _min.height);
}

const Cartographic Region::bottomLeft() const noexcept
{
    return Cartographic(_min.latitude, _min.longitude, _min.height);
}

const Cartographic Region::bottomRight() const noexcept
{
    return Cartographic(_min.latitude, _max.longitude, _min.height);
}

Region Region::northWest() const noexcept
{
    double w = width() / 2;
    double h = height() / 2;
    return Region(Cartographic(south() + h, west()), w, h);
}

Region Region::northEast() const noexcept
{
    double w = width() / 2;
    double h = height() / 2;
    return Region(Cartographic(south() + h, west() + w), w, h);
}

Region Region::southWest() const noexcept
{
    double w = width() / 2;
    double h = height() / 2;
    return Region(Cartographic(south(), west()), w, h);
}

Region Region::southEast() const noexcept
{
    double w = width() / 2;
    double h = height() / 2;
    return Region(Cartographic(south(), west() + w), w, h);
}

Region Region::world()
{
    return Region(Cartographic::fromDegrees(-90, -180, 0), Cartographic::fromDegrees(90, 180, 0));
}
