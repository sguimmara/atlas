#include "math.hpp"

using namespace atlas::core;

double Math::lerp(double a, double b, double t)
{
    return a + (b - a) * t;
}
