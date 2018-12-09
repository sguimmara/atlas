#include "ellipsoid.hpp"

using namespace atlas::core;
using namespace atlas::core::srs;

Ellipsoid::Ellipsoid() : _semimajorAxis(0), _semiminorAxis(0)
{}

Ellipsoid::Ellipsoid(double semimajorAxis, double semiminorAxis) :
    _semimajorAxis(semimajorAxis),
    _semiminorAxis(semiminorAxis)
{
}

Ellipsoid Ellipsoid::WGS84()
{
    return Ellipsoid(6378137, 6356752.314245);
}

Ellipsoid Ellipsoid::unitSphere()
{
    return Ellipsoid(1, 1);
}

f64vec3 Ellipsoid::position(double lat, double lon, double height) const noexcept
{
    // Implementation taken from the book "3D Engine design
    // for virtual globes", by Patrick Cozzi and Kevin Ring.

    const double a = _semimajorAxis;
    const double b = _semimajorAxis;
    const double c = _semiminorAxis;

    // First, compute the surface normal vector.
    const f64vec3 normal = normalDouble(lat, lon, height);

    double gamma = std::sqrt(
        (a*a*normal.x*normal.x) +
        (b*b*normal.y*normal.y) +
        (c*c*normal.z*normal.z));

    const double xS = (a * a * normal.x) / gamma;
    const double yS = (b * b * normal.y) / gamma;
    const double zS = (c * c * normal.z) / gamma;

    // then compute the surface point
    const f64vec3 surface(xS, zS, yS);

    if (height == 0)
    {
        return surface;
    }

    // the point is not on the surface, just offset the surface point by the normal.
    f64vec3 h = height * normal;

    return surface + h;
}

inline f64vec3 Ellipsoid::position(const Cartographic& p) const noexcept
{
    return position(p.latitude, p.longitude, p.height);
}

f64vec3 Ellipsoid::normalDouble(double lat, double lon, double height) const noexcept
{
    const double theta = lat;
    const double lambda = lon;

    const double cosT = std::cos(theta);
    const double cosL = std::cos(lambda);
    const double sinL = std::sin(lambda);
    const double sinT = std::sin(theta);

    const f64vec3 i(1, 0, 0);
    const f64vec3 j(0, 1, 0);
    const f64vec3 k(0, 0, 1);

    // First, compute the surface normal vector.
    return (cosT * cosL * i) + (cosT * sinL * j) + (sinT * k);
}

vec3 Ellipsoid::normal(double lat, double lon) const noexcept
{
    return (vec3)normalDouble(lat, lon, 0);
}
