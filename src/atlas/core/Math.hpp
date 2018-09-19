#ifndef ATLAS_MATH_HPP
#define ATLAS_MATH_HPP

#include <vector>
#include <string>

#include "Constants.hpp"
#include "Plane.hpp"

using namespace atlas::core;

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

            static vec3 PlaneLineIntersection(vec3 p0, vec3 p1, Plane plane)
            {
                vec3 line = p1 - p0;
                auto dot0 = glm::dot(plane.normal, p0);
                auto dot1 = glm::dot(plane.normal, line);
                return p0 + line * static_cast<float>((((plane.distance - dot0) / dot1)));
            }
        };
    }
}
#endif