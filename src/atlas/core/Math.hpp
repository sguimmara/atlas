#ifndef ATLAS_MATH_HPP
#define ATLAS_MATH_HPP

#include <vector>
#include <string>

#include "Constants.hpp"

namespace atlas
{
    namespace core
    {
        /**
        * @brief      Common mathematical functions.
        */
        class Math
        {
        public:
            static inline double ToRadians(double degrees) noexcept
            {
                return degrees * PI / 180;
            }
            static inline double ToDegrees(double radians) noexcept
            {
                return radians * 180 / PI;
            }
        };
    }
}
#endif