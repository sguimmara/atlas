#ifndef ATLAS_CORE_COLOR_HPP
#define ATLAS_CORE_COLOR_HPP

#include <cstdint>
#include <iostream>

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

        /**
        * @brief      Pure red
        */
        static const Color red;

        /**
        * @brief      Pure green
        */
        static const Color green;

        /**
        * @brief      Pure blue
        */
        static const Color blue;

        /**
        * @brief      Pure white
        */
        static const Color white;


        Color() : r(0), g(0), b(0), a(0) {}

        Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) :
            r(r), g(g), b(b), a(a)
        {
        }

        bool operator ==(const Color& rhs) const noexcept;
        bool operator !=(const Color& rhs) const noexcept;

        /**
        * @brief      Multiply by a constant.
        */
        Color operator *(const float rhs);

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

    const Color Color::red = { 255, 0, 0, 255 };
    const Color Color::green = { 0, 255, 0, 255 };
    const Color Color::blue = { 0, 0, 255, 255 };
    const Color Color::white = { 255, 255, 255, 255 };
}

#endif