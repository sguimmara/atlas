#ifndef ATLAS_GRAPHICS_PRIMITIVES_POINT
#define ATLAS_GRAPHICS_PRIMITIVES_POINT

#include "atlas/graphics/MeshObject.hpp"
#include "atlas/graphics/Renderer.hpp"

namespace atlas {
    namespace graphics {
        namespace primitives
        {
            /**
            * @brief      A Point is represented by a single pixel on screen.
            */
            struct Point : atlas::graphics::MeshObject
            {
                Point(glm::vec3 position);
            };
        }
    }
}

#endif