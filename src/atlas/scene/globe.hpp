#pragma once
#ifndef ATLAS_SCENE_GLOBE_HPP
#define ATLAS_SCENE_GLOBE_HPP

#include "atlas/core/srs/spatialreference.hpp"
#include "atlas/core/spatialindex/quadtree.hpp"
#include "atlas/io/imagesource.hpp"
#include "atlas/renderer/material.hpp"
#include "globeevaluator.hpp"
#include "entity.hpp"
#include "layer.hpp"
#include "tile.hpp"
#include <unordered_map>

namespace atlas::scene
{
    using namespace atlas::core;
    using namespace atlas::core::spatialindex;
    using namespace atlas::core::srs;
    using namespace atlas::io;
    using namespace atlas::renderer;

    // manages the rendering of the globe surface
    class Globe final : public Layer
    {
    public:
        Globe();
        Globe(Globe&) = delete;
        Globe(Globe&&) = delete;
        Globe(SpatialReference*);

        // Sets the current image source for the globe.
        void setImageSource(ImageSource*);

        // performs an update cycle :
        // 1. the quadtree is updated with the view position
        // 2. newly created terrain tiles are generated
        // 3. imagery is fetched from the datasources
        void update() override;

        std::vector<const Entity*> entities() const override;
        std::vector<const Entity*> debugEntities() const override;

    private:
        SpatialReference* _srs;
        GlobeEvaluator _evaluator;
        std::unique_ptr<Image> _defaultImage;
        std::vector<std::future<Response<Image>>> _imageRequests;
        std::unique_ptr<Quadtree> _quadtree;
        ImageSource* _imageSource;
        std::unordered_map<QuadtreeNode::Key, std::shared_ptr<Tile>> _tiles;

        void updateQuadtree();
        void processPendingImages();
    };
}

#endif // ATLAS_SCENE_GLOBE_HPP