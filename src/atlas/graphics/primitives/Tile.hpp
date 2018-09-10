#ifndef ATLAS_GRAPHICS_PRIMITIVES_TILE
#define ATLAS_GRAPHICS_PRIMITIVES_TILE

#include "atlas/graphics/MeshObject.hpp"
#include "atlas/graphics/Renderer.hpp"

namespace atlas {
    namespace graphics {
        namespace primitives
        {
            /**
            * @brief      A tile is a section of the earth surface, in the WGS84 ellipsoid.
            */
            struct Tile : MeshObject
            {
                uint32_t subdivs;

                Tile(vk::PhysicalDevice gpu, vk::Device device, uint16_t subdivs, glm::vec2 min, glm::vec2 max);

                static Tile Create(Renderer* renderer, uint16_t subdivs, int level, int row, int col);
            };
        }
    }
}

#endif