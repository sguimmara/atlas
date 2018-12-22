#ifndef ATLAS_CORE_SPATIALREFERENCE_HPP
#define ATLAS_CORE_SPATIALREFERENCE_HPP

#include "../common.hpp"
#include "../cartographic.hpp"

namespace atlas::core::srs
{
    class SpatialReference
    {
    public:
        virtual ~SpatialReference() {}
        virtual f64vec3 position(const Cartographic&) const noexcept = 0;
        virtual f64vec3 position(double lat, double lon, double height = 0) const noexcept = 0;
        virtual vec3 normal(double lat, double lon) const noexcept = 0;
    };
}

#endif // ATLAS_CORE_SPATIALREFERENCE_HPP