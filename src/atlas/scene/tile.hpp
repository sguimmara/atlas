#ifndef ATLAS_SCENE_TILE_HPP
#define ATLAS_SCENE_TILE_HPP

#include "common.hpp"
#include "entity.hpp"
#include "atlas/core/quadtreenode.hpp"
#include "atlas/core/ellipsoid.hpp"
#include "atlas/core/region.hpp"
#include "atlas/core/image.hpp"
#include "atlas/renderer/texture.hpp"

namespace atlas::scene
{
    using namespace atlas::core;
    using namespace atlas::renderer;

    class Tile
    {
    public:
        Tile(Region, const Ellipsoid&);

        inline const Entity* entity() const noexcept { return _entity.get(); }
        inline const Region region() const noexcept { return _region; }
        void setImage(const Image*);

    private:
        std::unique_ptr<Texture> _image;
        Region _region;
        std::unique_ptr<Entity> _entity;
    };
}

#endif // ATLAS_SCENE_TILE_HPP