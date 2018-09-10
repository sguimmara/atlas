#include "lest/lest.hpp"

#include "atlas/core/Atlas.hpp"
#include "atlas/graphics/AtlasGraphics.hpp"
#include "atlas/graphics/Renderer.hpp"

using namespace atlas::graphics;

const lest::test specification[] =
{
    {
        CASE("renderer")
        {
            atlas::Atlas app;
        }
    },

};

int main(int argc, char * argv[])
{
    return lest::run(specification, argc, argv);
}
