#ifndef ATLAS_GRAPHICS_PRIMITIVES_TILE
#define ATLAS_GRAPHICS_PRIMITIVES_TILE

#include "atlas/graphics/Mesh.hpp"

namespace atlas {
    namespace graphics {
        namespace primitives
        {
            struct Tile : Mesh
            {
                uint32_t subdivs;

                Tile(vk::PhysicalDevice gpu, vk::Device device, uint16_t subdivs, glm::vec2 min, glm::vec2 max);
            };
        }
    }
}

#endif