#ifndef ATLAS_GRAPHICS_OBJECTS_EARTH
#define ATLAS_GRAPHICS_OBJECTS_EARTH

#include "atlas/core/Ellipsoid.hpp"

#include "atlas/graphics/AtlasGraphics.hpp"
#include "atlas/graphics/Node.hpp"
#include "atlas/graphics/Renderer.hpp"

using namespace atlas::core;

namespace atlas
{
    namespace graphics
    {
        namespace objects
        {
            class Earth : public Node
            {
            public:

                Earth(Ellipsoid ellipsoid, double scale);
                ~Earth();
            private:
                Ellipsoid _ellipsoid;
                double _scale;
            };
        }
    }
}

#endif