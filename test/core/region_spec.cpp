#include "../common.hpp"
#include "atlas/core/region.hpp"
#include "atlas/core/cartographic.hpp"

using namespace lest;
using namespace atlas::core;

CASE( "Default constructor initializes creates empty region" "[pass]" )
{
    Region region = Region();

    auto zero = Cartographic();

    EXPECT(region.min() == zero);
    EXPECT(region.max() == region.min());
}

CASE("Cartographic constructor creates correct min and max values" "[pass]")
{
    Region region = Region(Cartographic(), Cartographic::fromDegrees(1, 1, 0));
    
    EXPECT(region.west() == approx(radians(0.0)));
    EXPECT(region.east() == approx(radians(1.0)));
    EXPECT(region.south() == approx(radians(0.0)));
    EXPECT(region.north() == approx(radians(1.0)));
    EXPECT(region.width() == approx(radians(1.0)));
    EXPECT(region.height() == approx(radians(1.0)));
}

CASE("Region::center() returns correct value" "[pass]")
{
    auto center = Region::world().center();

    EXPECT(center.latitude == 0);
    EXPECT(center.longitude == 0);
    EXPECT(center.height == 0);
}

CASE("Region::expand() creates correct union" "[pass]")
{
    Region a = Region(Cartographic(), Cartographic::fromDegrees(1, 1, 0));
    Region b = Region(
        Cartographic::fromDegrees(1, 1, 0),
        Cartographic::fromDegrees(2, 2, 0));

    a.expand(b);

    EXPECT(a.west() == approx(radians(0.0)));
    EXPECT(a.east() == approx(radians(2.0)));
    EXPECT(a.south() == approx(radians(0.0)));
    EXPECT(a.north() == approx(radians(2.0)));
    EXPECT(a.width() == approx(radians(2.0)));
    EXPECT(a.height() == approx(radians(2.0)));
}