#include "cartographic.hpp"

using namespace atlas::core;

Cartographic::Cartographic() :
    latitude(0),
    longitude(0),
    height(0)
{
}

Cartographic::Cartographic(double latitude, double longitude, double height) :
    latitude(latitude),
    longitude(longitude),
    height(height)
{
}

Cartographic Cartographic::fromDegrees(double latitude, double longitude, double height)
{
    return Cartographic(latitude * DegToRad, longitude * DegToRad, height);
}

Cartographic Cartographic::midpoint(const Cartographic& start, const Cartographic& end)
{
    double lat = start.latitude + (end.latitude - start.latitude) * 0.5;
    double lon = start.longitude + (end.longitude - start.longitude) * 0.5;
    double hgt = start.height + (end.height - start.height) * 0.5;

    return Cartographic(lat, lon, hgt);
}
