#include <algorithm>

#include "Color.hpp"

namespace atlas
{
    bool Color::operator ==(const Color& rhs) const noexcept
    {
        return rhs.r == r
            && rhs.g == g
            && rhs.b == b
            && rhs.a == a;
    }

    bool Color::operator !=(const Color& rhs) const noexcept
    {
        return rhs.r != r
            || rhs.g != g
            || rhs.b != b
            || rhs.a != a;
    }

    float clamp01(float x)
    {
        return std::max(0.0f, std::min(x, 1.0f));
    }

    Color Color::operator *(const float rhs) const noexcept
    {
        float f = clamp01(rhs);

        auto r1 = static_cast<uint8_t>(std::round(r * f));
        auto g1 = static_cast<uint8_t>(std::round(g * f));
        auto b1 = static_cast<uint8_t>(std::round(b * f));
        auto a1 = static_cast<uint8_t>(std::round(a * f));

        return Color(r1, g1, b1, a1);
    }

    Color Color::white = { 255,255,255,255 };
    Color Color::black = { 0,0,0,255 };
    Color Color::clear = { 0,0,0,0 };
    Color Color::green = { 0,255,0,255 };
    Color Color::yellow = { 255,255,0,255 };
    Color Color::cyan = { 0,255,255,255 };
    Color Color::gray = { 127,127,127,255 };
    Color Color::red = { 255,0,0,255 };
    Color Color::blue = { 0,0,255,255 };
}