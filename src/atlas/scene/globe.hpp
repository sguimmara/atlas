#ifndef ATLAS_SCENE_GLOBE_HPP
#define ATLAS_SCENE_GLOBE_HPP

#include "atlas/core/ellipsoid.hpp"
#include "atlas/core/quadtree.hpp"
#include "atlas/io/imagesource.hpp"
#include "atlas/renderer/material.hpp"
#include "entity.hpp"
#include "tile.hpp"
#include <unordered_map>

namespace atlas::scene
{
    using namespace atlas::core;
    using namespace atlas::io;
    using namespace atlas::renderer;

    // manages the rendering of the globe surface
    class Globe
    {
    public:
        Globe();
        Globe(Globe&) = delete;
        Globe(Globe&&) = delete;
        Globe(Ellipsoid);

        // performs an update cycle :
        // 1. the quadtree is updated with the view position
        // 2. newly created terrain tiles are generated
        // 3. imagery is fetched from the datasources
        void update();

        std::vector<const Entity*> tiles() noexcept;

    private:
        Ellipsoid _ellipsoid;
        std::vector<std::future<Response<Image>>> _imageRequests;
        std::unique_ptr<Quadtree> _quadtree;
        std::shared_ptr<ImageSource> _imageSource;
        std::unordered_map<QuadtreeNode::Key, std::unique_ptr<Tile>> _tiles;
    };
}

#endif // ATLAS_SCENE_GLOBE_HPP