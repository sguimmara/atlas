#ifndef ATLAS_GRAPHICS_DEBUG_POINT
#define ATLAS_GRAPHICS_DEBUG_POINT

#include "atlas/graphics/AtlasGraphics.hpp"
#include "atlas/graphics/Renderer.hpp"
#include "atlas/graphics/Mesh.hpp"

namespace atlas
{
    namespace graphics
    {
        namespace debug
        {
            class Point : public Mesh
            {
            public:
                Point(glm::vec3 position);
            };
        }
    }
}

#endif