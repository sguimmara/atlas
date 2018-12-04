#ifndef ATLAS_SCENE_GLOBE_HPP
#define ATLAS_SCENE_GLOBE_HPP

#include "atlas/core/quadtree.hpp"
#include "atlas/renderer/material.hpp"
#include "entity.hpp"

namespace atlas::scene
{
    using namespace atlas::core;
    using namespace atlas::renderer;

    // manages the rendering of the globe surface
    class Globe
    {
    public:
        Globe();

        std::vector<Entity*> tiles() noexcept;

    private:
        std::unique_ptr<Quadtree> _quadtree;
        std::unique_ptr<Texture> _TEMPtexture;
        std::vector<std::shared_ptr<Entity>> _tiles;

        std::unique_ptr<Material> _TEMPterrainMaterial;
    };
}

#endif // ATLAS_SCENE_GLOBE_HPP