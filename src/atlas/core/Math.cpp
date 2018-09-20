#include "Math.hpp"

vec3 Math::LatLonToECEF(double lat, double lon, Ellipsoid& ellipsoid)
{
    const double scale = 0.0000005;

    const double a = ellipsoid.semimajorAxis() * scale;
    const double b = ellipsoid.semiminorAxis() * scale;
    const double e2 = 1 - (b * b) / (a * a);

    double sinlat = std::sin(lat);
    double sinlon = std::sin(lon);
    double coslon = std::cos(lon);
    double coslat = std::cos(lat);

    double nLat = a / std::sqrt(1 - e2 * sinlat);

    double x = (nLat)* coslat * coslon;
    double z = (nLat)* coslat * sinlon;
    double y = (((b*b) / (a*a)*nLat)) * sinlat;

    return vec3{ x, z, y };
}