#include "../../common.hpp"
#include "atlas/core/srs/equirectangular.hpp"
#include "atlas/core/cartographic.hpp"

using namespace lest;
using namespace atlas::core;
using namespace atlas::core::srs;

CASE("Equirectangular::position()")
{
    Equirectangular proj;

    auto const pos = proj.position(Cartographic(0, 0, 0));
    EXPECT(pos.x == approx(0));
    EXPECT(pos.y == approx(0));
    EXPECT(pos.z == approx(0));
}
