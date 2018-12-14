#ifndef ATLAS_CORE_ELLIPSOID_HPP
#define ATLAS_CORE_ELLIPSOID_HPP

#include "../common.hpp"
#include "../cartographic.hpp"
#include "spatialreference.hpp"

namespace atlas::core::srs
{
    class Ellipsoid final : public SpatialReference
    {
    public:
        Ellipsoid();
        Ellipsoid(meters semimajorAxis, meters semiminorAxis);
        inline double semimajorAxis() const noexcept { return _semimajorAxis; }
        inline double semiminorAxis() const noexcept { return _semiminorAxis; }
        static Ellipsoid WGS84();
        static Ellipsoid unitSphere();
        static Ellipsoid spherical(double radius);

        f64vec3 position(rad lat, rad lon, meters height = 0) const noexcept override;
        f64vec3 position(const Cartographic& p) const noexcept override;
        vec3 normal(rad lat, rad lon) const noexcept override;

        f64vec3 normalDouble(rad lat, rad lon, meters height = 0) const noexcept;

    private:
        meters _semimajorAxis;
        meters _semiminorAxis;
        f64vec3 _radiiSquared;
    };
}

#endif