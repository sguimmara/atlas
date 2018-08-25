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
}