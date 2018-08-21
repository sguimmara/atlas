#include "lest/lest.hpp"

#include "atlas/core/Cartographic.hpp"

using namespace atlas::core;

const lest::test specification[] =
{
    {
        CASE("default constructor initializes to zero")
        {
            Cartographic g;
            EXPECT(g.longitude() == 0);
            EXPECT(g.latitude() == 0);
            EXPECT(g.altitude() == 0);
        }
    },
    {
        CASE("constructor initializes correct values")
        {
            Cartographic g(23, 1.323, 1193193.313);
            EXPECT(g.longitude() == 23);
            EXPECT(g.latitude() == 1.323);
            EXPECT(g.altitude() == 1193193.313);
        }
    },
    {
        CASE("constructor default initializes altitude to zero")
        {
            Cartographic g(1, 2);
            EXPECT(g.altitude() == 0);
        }
    },
};

int main(int argc, char * argv[])
{
    return lest::run(specification, argc, argv);
}