#ifndef ATLAS_GRAPHICS_VERTEX_HPP
#define ATLAS_GRAPHICS_VERTEX_HPP

#include "AtlasGraphics.hpp"

namespace atlas
{
    namespace graphics
    {
        struct Vertex
        {
            glm::vec3 position;
            glm::vec3 normal;
            uint32_t uv;
        };
    }
}

#endif