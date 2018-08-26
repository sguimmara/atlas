#include "lest/lest.hpp"

#include "atlas/core/Color.hpp"

using namespace atlas;

const lest::test specification[] =
{
    {
        CASE("default constructor initializes to zero")
        {
            Color color;
            EXPECT(color.a == 0);
            EXPECT(color.r == 0);
            EXPECT(color.g == 0);
            EXPECT(color.b == 0);
        }
    },
    {
        CASE("Color(0, 0, 0, 0)")
        {
            Color color(0, 0, 0, 0);
            EXPECT(color.a == 0);
            EXPECT(color.r == 0);
            EXPECT(color.g == 0);
            EXPECT(color.b == 0);
        }
    },
    {
        CASE("Color(255, 255, 255, 255)")
        {
            Color color(255, 255, 255, 255);
            EXPECT(color.a == 255);
            EXPECT(color.r == 255);
            EXPECT(color.g == 255);
            EXPECT(color.b == 255);
        }
    },
    {
        CASE("operator ==")
        {
            int expected = 255;
            int actual = 0;

            for (int i = 0; i < 255; ++i)
            {
                uint8_t v = static_cast<uint8_t>(i);
                Color c1(v, v, v, v);
                Color c2(v, v, v, v);
                if (c1 == c2)
                {
                    actual++;
                }
            }
            EXPECT(actual == expected);
        }
    },
    {
        CASE("operator !=")
        {
            int expected = 255;
            int actual = 0;

            for (int i = 0; i < 255; ++i)
            {
                uint8_t v = static_cast<uint8_t>(i);
                auto u = (v + 1) % 256;

                Color c1(v, v, v, v);
                Color c2(u, u, u, u);
                if (c1 != c2)
                {
                    actual++;
                }
            }
            EXPECT(actual == expected);
        }
    },
    {
        CASE("operator *")
        {
            int expected = 255;
            int actual = 0;

            Color c(0, 0, 0, 0);
            EXPECT((c * 0) == c);
            EXPECT((c * 0.7f) == c);
            EXPECT((c * 1.0f) == c);

            Color c1(255, 255, 255, 255);
            EXPECT((c1 * 1.0f) == c1);
            EXPECT((c1 * 0.5f) == Color(128, 128, 128, 128));
            EXPECT((c1 * 2.0f) == c1);
        }
    },
};

int main(int argc, char * argv[])
{
    return lest::run(specification, argc, argv);
}