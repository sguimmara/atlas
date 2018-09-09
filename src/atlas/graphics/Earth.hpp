#ifndef ATLAS_GRAPHICS_EARTH_HPP
#define ATLAS_GRAPHICS_EARTH_HPP

#include "AtlasGraphics.hpp"
#include "Node.hpp"
#include "SurfaceTile.hpp"

namespace atlas
{
    namespace graphics
    {
        /**
        * @brief      The Earth node manages the rendering of the surface of the earth.
        */
        class Earth : public Node
        {
        public:
            Earth(Renderer* renderer);
            ~Earth();
        };
    }
}

#endif