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

        static double dmsToDD(double d, double m, double s);
        static Cartographic fromDMS(double latD, double latM, double latS,
                                    double lonD, double lonM, double lonS,
                                    double height = 0);
        static Cartographic fromDegrees(double latitude, double longitude, double height = 0);
        static Cartographic midpoint(const Cartographic& start, const Cartographic& end);
        static Cartographic lerp(const Cartographic& from, const Cartographic& to, double t);
    };
}

#endif