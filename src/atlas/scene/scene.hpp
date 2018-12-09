#ifndef ATLAS_SCENE_SCENE_HPP
#define ATLAS_SCENE_SCENE_HPP

#include "atlas/core/srs/spatialreference.hpp"
#include "atlas/core/srs/ellipsoid.hpp"
#include "atlas/core/srs/equirectangular.hpp"
#include "atlas/renderer/globalproperties.hpp"
#include "atlas/renderer/instance.hpp"
#include "atlas/renderer/pipeline.hpp"
#include "atlas/renderer/context.hpp"
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

    class Scene
    {
    public:
        Scene(std::string name);
        ~Scene();
        void addEntity(Entity*);
        void render(const Time&);
    private:
        std::string _name;
        std::unique_ptr<SpatialReference> _srs;
        std::shared_ptr<spdlog::logger> _log;

        std::unique_ptr<Globe> _globe;
        std::vector<std::unique_ptr<Entity>> _entities;
        std::vector<std::unique_ptr<View>> _views;

        void setupView(View&, const Time&);
        void renderGlobe(atlas::renderer::Context*, View&);
        void drawEntity(atlas::renderer::Context* ctx, const Entity& entity, View& view);
    };
}

#endif // ATLAS_SCENE_SCENE_HPP