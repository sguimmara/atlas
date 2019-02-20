#pragma once
#ifndef ATLAS_SCENE_SCENE_HPP
#define ATLAS_SCENE_SCENE_HPP

#include "atlas/core/srs/spatialreference.hpp"
#include "atlas/core/srs/ellipsoid.hpp"
#include "atlas/core/srs/equirectangular.hpp"
#include "atlas/renderer/globalproperties.hpp"
#include "atlas/renderer/instance.hpp"
#include "atlas/renderer/pipeline.hpp"
#include "atlas/renderer/context.hpp"
#include "atlas/io/fileimagesource.hpp"
#include "atlas/io/debugimagesource.hpp"
#include "entity.hpp"
#include "globe.hpp"
#include "view.hpp"
#include "time.hpp"
#include "camera.hpp"
#include <vector>
#include <unordered_map>

namespace atlas::scene
{
    using namespace atlas::core::srs;
    using namespace atlas::io;

    class Scene
    {
    public:
        Scene(std::string name);
        ~Scene();
        void render(const Time&);

        Globe* globe() const noexcept;

        // Returns a pointer to the layer with the given name if any, null otherwise.
        Layer* getLayer(const std::string&) const;
    private:
        std::string _name;
        std::unique_ptr<SpatialReference> _srs;
        std::shared_ptr<spdlog::logger> _log;

        std::vector<std::unique_ptr<Layer>> _layers;
        std::vector<std::unique_ptr<View>> _views;

        std::unique_ptr<ImageSource> _TMP_ImageSource;

        void setupView(View&, const Time&);
        void drawEntity(atlas::renderer::Context* ctx, const Entity& entity, const View& view);
        void renderList(const std::vector<const Entity*>& queue, const View& view);
    };
}

#endif // ATLAS_SCENE_SCENE_HPP