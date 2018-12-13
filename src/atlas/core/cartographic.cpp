#include "cartographic.hpp"
#include "math.hpp"

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

double Cartographic::dmsToDD(double d, double m, double s)
{
    return d + m / 60 + s / 3600;
}

Cartographic Cartographic::fromDMS(double latD, double latM, double latS, double lonD, double lonM, double lonS, double height)
{
    return Cartographic(dmsToDD(latD, latM, latS), dmsToDD(lonD, lonM, lonS), height);
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

Cartographic Cartographic::lerp(const Cartographic& from, const Cartographic& to, double t)
{
    double lat = Math::lerp(from.latitude, to.latitude, t);
    double lon = Math::lerp(from.longitude, to.longitude, t);
    double hgt = Math::lerp(from.height, to.height, t);

    return Cartographic(lat, lon, hgt);
}
