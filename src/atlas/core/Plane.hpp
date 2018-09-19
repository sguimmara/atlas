#ifndef ATLAS_PLANE_HPP
#define ATLAS_PLANE_HPP

#include <vector>
#include <string>

#include "Constants.hpp"

namespace atlas
{
    namespace core
    {
        /**
        * @brief      A plane.
        */
        struct Plane
        {
            vec3 normal;
            double distance;
        };
    }
}

#endif