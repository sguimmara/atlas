#include "../common.hpp"
#include "atlas/core/cartographic.hpp"

using namespace atlas::core;

CASE( "Default constructor initializes lat,lon,height at zero" "[pass]" )
{
    Cartographic point = Cartographic();

    EXPECT(point.latitude == 0);
    EXPECT(point.longitude == 0);
    EXPECT(point.height == 0);
}

CASE("Parametric constructor initializes lat,lon,height at given values" "[pass]")
{
    Cartographic point = Cartographic(0, 0, 0);

    EXPECT(point.latitude == 0);
    EXPECT(point.longitude == 0);
    EXPECT(point.height == 0);
}

CASE("Parametric constructor initializes lat,lon,height at given values 2" "[pass]")
{
    Cartographic point = Cartographic(25.7, 7, 14000);

    EXPECT(point.latitude == 25.7);
    EXPECT(point.longitude == 7);
    EXPECT(point.height == 14000);
}

CASE("fromDegrees() performs correct conversion 1" "[pass]")
{
    Cartographic point = Cartographic::fromDegrees(0, 0, 0);

    EXPECT(point.latitude == 0);
    EXPECT(point.longitude == 0);
    EXPECT(point.height == 0);
}

CASE("fromDegrees() performs correct conversion 2" "[pass]")
{
    Cartographic point = Cartographic::fromDegrees(90, 180, 14000);

    EXPECT(point.latitude == HALF_PI);
    EXPECT(point.longitude == PI);
    EXPECT(point.height == 14000);
}

CASE("fromDMS() performs correct conversion" "[pass]")
{
    Cartographic point = Cartographic::fromDMS(0, 0, 0, 0, 0, 0, 0);

    EXPECT(point.latitude == 0);
    EXPECT(point.longitude == 0);
    EXPECT(point.height == 0);
}

CASE("fromDMS() performs correct conversion 2" "[pass]")
{
    Cartographic point = Cartographic::fromDMS(45, 5, 22, 170, 25, 34, 98589);

    EXPECT(degrees(point.latitude) - 45.08944 < 0.01);
    EXPECT(degrees(point.longitude) - 170.4261 < 0.01);
    EXPECT(point.height == 98589);
}