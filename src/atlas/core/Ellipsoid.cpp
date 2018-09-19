#include "Ellipsoid.hpp"

using namespace atlas::core;

Ellipsoid::Ellipsoid(double semimajorAxis, double semiminorAxis) :
    _semimajorAxis(semimajorAxis),
    _semiminorAxis(semiminorAxis)
{

}

Ellipsoid Ellipsoid::GRS80 = Ellipsoid(6378137, 6356752.314140347);