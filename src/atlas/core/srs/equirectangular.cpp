#include "equirectangular.hpp"

using namespace atlas::core;
using namespace atlas::core::srs;

f64vec3 Equirectangular::position(const Cartographic& p) const noexcept
{
    return f64vec3(0, p.longitude, p.latitude);
}

f64vec3 Equirectangular::position(double lat, double lon, double height) const noexcept
{
    return f64vec3(0, lon, lat);
}

vec3 Equirectangular::normal(double lat, double lon) const noexcept
{
    return vec3(1, 0, 0);
}
