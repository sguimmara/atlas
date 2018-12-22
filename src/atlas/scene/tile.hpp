#pragma once
#ifndef ATLAS_SCENE_TILE_HPP
#define ATLAS_SCENE_TILE_HPP

#include "common.hpp"
#include "entity.hpp"
#include "atlas/core/srs/spatialreference.hpp"
#include "atlas/core/region.hpp"
#include "atlas/core/image.hpp"
#include "atlas/renderer/texture.hpp"

namespace atlas::scene
{
    using namespace atlas::core;
    using namespace atlas::core::srs;
    using namespace atlas::renderer;

    class Tile
    {
    public:
        Tile(Region, const SpatialReference&);

        inline void setVisible(bool v) noexcept { _visible = v; }
        inline bool visible() const noexcept { return _visible; }
        inline const Entity* debugEntity() const noexcept { return _debugEntity.get(); }
        inline const Entity* entity() const noexcept { return _entity.get(); }
        inline const Region region() const noexcept { return _region; }
        void setImage(const Image*);

    private:
        bool _visible;
        std::unique_ptr<Texture> _image;
        Region _region;
        std::unique_ptr<Entity> _entity;
        std::shared_ptr<Entity> _debugEntity;
    };
}

#endif // ATLAS_SCENE_TILE_HPP