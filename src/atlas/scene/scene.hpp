#ifndef ATLAS_SCENE_SCENE_HPP
#define ATLAS_SCENE_SCENE_HPP

#include "atlas/renderer/global_properties.hpp"
#include "atlas/renderer/instance.hpp"
#include "atlas/renderer/pipeline.hpp"
#include "atlas/renderer/context.hpp"
#include "entity.hpp"
#include "globe.hpp"
#include "view.hpp"
#include "camera.hpp"
#include <vector>
#include <unordered_map>

namespace atlas::scene
{
    class Scene
    {
    public:
        Scene(std::string name);
        ~Scene();
        void addEntity(Entity*);
        void render();
    private:
        std::string _name;
        std::shared_ptr<spdlog::logger> _log;

        std::unique_ptr<Globe> _globe;
        std::vector<std::unique_ptr<View>> _views;

        std::vector<Entity*> _entities;

        void setupView(View& view);
        void renderGlobe(atlas::renderer::Context*, View&);
    };
}

#endif // ATLAS_SCENE_SCENE_HPP