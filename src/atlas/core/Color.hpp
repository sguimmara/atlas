#ifndef ATLAS_CORE_COLOR_HPP
#define ATLAS_CORE_COLOR_HPP

#include <cstdint>
#include <iostream>

#include <glm/glm.hpp>

using namespace glm;

namespace atlas
{
    /**
     * @brief      A 8-bit per channel RGBA color.
     */
    struct Color
    {
        /**
        * @brief      red component
        */
        uint8_t r;
        /**
        * @brief      green component
        */
        uint8_t g;
        /**
        * @brief      blue component
        */
        uint8_t b;
        /**
        * @brief      alpha component
        */
        uint8_t a;

        Color() : r(0), g(0), b(0), a(0) {}

        Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) :
            r(r), g(g), b(b), a(a)
        {
        }

        bool operator ==(const Color& rhs) const noexcept;
        bool operator !=(const Color& rhs) const noexcept;

        operator vec3() const { return vec3(r / 255.0f, g / 255.0f, b / 255.0f); }

        /**
        * @brief      Multiply by a constant.
        */
        Color operator *(const float rhs) const noexcept;

        static Color red;
        static Color blue;
        static Color white;
        static Color black;
        static Color clear;
        static Color green;
        static Color yellow;
        static Color cyan;
        static Color gray;

        friend std::ostream& operator<< (
            std::ostream& os,
            const Color& c)
        {
            return os << "RGBA ["
                << static_cast<int>(c.r) << " "
                << static_cast<int>(c.g) << " "
                << static_cast<int>(c.b) << " "
                << static_cast<int>(c.a) << "]";
        }
    };
}

#endif