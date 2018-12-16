#include "cartographic.hpp"
#include "math.hpp"

using namespace atlas::core;

Cartographic::Cartographic() :
    latitude(0),
    longitude(0),
    height(0)
{
}

Cartographic::Cartographic(rad latitude, rad longitude, meters height) :
    latitude(latitude),
    longitude(longitude),
    height(height)
{
}

double Cartographic::dmsToDD(deg d, deg m, deg s)
{
    return d + m / 60 + s / 3600;
}

Cartographic Cartographic::fromDMS(deg latD, deg latM, deg latS, deg lonD, deg lonM, deg lonS, meters height)
{
    return Cartographic(radians(dmsToDD(latD, latM, latS)), radians(dmsToDD(lonD, lonM, lonS)), height);
}

Cartographic Cartographic::fromDegrees(deg latitude, deg longitude, meters height)
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

Cartographic Cartographic::lerp(const Cartographic& from, const Cartographic& to, double t)
{
    double lat = Math::lerp(from.latitude, to.latitude, t);
    double lon = Math::lerp(from.longitude, to.longitude, t);
    double hgt = Math::lerp(from.height, to.height, t);

    return Cartographic(lat, lon, hgt);
}
