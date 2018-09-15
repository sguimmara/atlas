#ifndef ATLAS_GRAPHICS_DEBUG_AXES
#define ATLAS_GRAPHICS_DEBUG_AXES

#include "atlas/graphics/AtlasGraphics.hpp"
#include "atlas/graphics/Renderer.hpp"
#include "atlas/graphics/Mesh.hpp"

namespace atlas
{
    namespace graphics
    {
        namespace debug
        {
            class Axes : public Mesh
            {
            public:
                Axes();
            };
        }
    }
}

#endif