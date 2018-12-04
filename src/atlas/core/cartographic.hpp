#ifndef ATLAS_CORE_CARTOGRAPHIC_HPP
#define ATLAS_CORE_CARTOGRAPHIC_HPP

#include "common.hpp"

namespace atlas::core
{
    struct Cartographic
    {
        double latitude;
        double longitude;
        double height;

        Cartographic();
        Cartographic(double latitude, double longitude, double height = 0);

        static Cartographic fromDegrees(double latitude, double longitude, double height);
        static Cartographic midpoint(const Cartographic& start, const Cartographic& end);
    };
}

#endif