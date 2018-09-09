#ifndef ATLAS_GRAPHICS_TIME_HPP
#define ATLAS_GRAPHICS_TIME_HPP

#include "AtlasGraphics.hpp"

namespace atlas
{
    namespace graphics
    {
        /**
        * @brief      Time related variables.
        */
        class Time
        {
        public:
            /**
            * @brief      The number of seconds elapsed since the previous frame.
            */
            static double dt;

            /**
            * @brief      The number of seconds elapsed since application startup.
            */
            static double elapsed;
        };
    }
}

#endif