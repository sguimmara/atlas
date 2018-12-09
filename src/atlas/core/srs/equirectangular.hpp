#ifndef ATLAS_CORE_EQUIRECTANGULAR_HPP
#define ATLAS_CORE_EQUIRECTANGULAR_HPP

#include "../common.hpp"
#include "../cartographic.hpp"
#include "spatialreference.hpp"

namespace atlas::core::srs
{
    using namespace atlas::core;

    // A planar, equirectangular projection (EPSG:4326).
    // Maps cartographic positions in the [-PI, PI] horizontal (X) range,
    // and [-PI/2, PI/2] vertical (Z) range.
    class Equirectangular final : public SpatialReference
    {
        f64vec3 position(const Cartographic&) const noexcept override;
        f64vec3 position(double lat, double lon, double height = 0) const noexcept override;
        vec3 normal(double lat, double lon) const noexcept override;
    };
}

#endif // ATLAS_CORE_EQUIRECTANGULAR_HPP