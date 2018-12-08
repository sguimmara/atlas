#ifndef ATLAS_CORE_ELLIPSOID_HPP
#define ATLAS_CORE_ELLIPSOID_HPP

#include "common.hpp"
#include "cartographic.hpp"

namespace atlas::core
{
    class Ellipsoid
    {
    public:
        Ellipsoid();
        Ellipsoid(double semimajorAxis, double semiminorAxis);
        inline double semimajorAxis() const noexcept { return _semimajorAxis; }
        inline double semiminorAxis() const noexcept { return _semiminorAxis; }
        static Ellipsoid WGS84();
        static Ellipsoid unitSphere();

        f64vec3 position(double lat, double lon, double height = 0) const noexcept;
        f64vec3 position(Cartographic p) const noexcept;

        f64vec3 normalDouble(double lat, double lon, double height = 0) const noexcept;

        /* return the normal vector for a given geographic position */
        vec3 normal(double lat, double lon) const noexcept;

    private:
        double _semimajorAxis;
        double _semiminorAxis;
    };
}

#endif