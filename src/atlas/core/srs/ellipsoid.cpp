#include "ellipsoid.hpp"

using namespace atlas::core;
using namespace atlas::core::srs;

Ellipsoid::Ellipsoid() : _semimajorAxis(0), _semiminorAxis(0)
{}

Ellipsoid::Ellipsoid(meters semimajorAxis, meters semiminorAxis) :
    _semimajorAxis(semimajorAxis),
    _semiminorAxis(semiminorAxis),
    _radiiSquared(semiminorAxis*semiminorAxis, semimajorAxis*semimajorAxis, semimajorAxis*semimajorAxis)
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

Ellipsoid Ellipsoid::spherical(double radius)
{
    return Ellipsoid(radius, radius);
}

f64vec3 Ellipsoid::position(rad lat, rad lon, meters height) const noexcept
{
    // Implementation taken from the book "3D Engine design
    // for virtual globes", by Patrick Cozzi and Kevin Ring.

    // First, compute the surface normal vector.
    const f64vec3 n = normalDouble(lat, lon, height);
    const f64vec3 k = _radiiSquared * n;
    double gamma = std::sqrt(k.x * n.x + k.y * n.y + k.z * n.z);
    const f64vec3 surface = k / gamma;
    f64vec3 result = surface + (height * n);
    return f64vec3{ result.x, result.z, result.y };
}

inline f64vec3 Ellipsoid::position(const Cartographic& p) const noexcept
{
    return position(p.latitude, p.longitude, p.height);
}

f64vec3 Ellipsoid::normalDouble(rad lat, rad lon, meters height) const noexcept
{
    const double cosLat = std::cos(lat);
    return f64vec3(cosLat * std::cos(lon), cosLat * std::sin(lon), std::sin(lat));
}

vec3 Ellipsoid::normal(rad lat, rad lon) const noexcept
{
    return (vec3)normalDouble(lat, lon, 0);
}
