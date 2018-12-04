#ifndef ATLAS_SCENE_SCENE_HPP
#define ATLAS_SCENE_SCENE_HPP

#include "atlas/renderer/global_properties.hpp"
#include "entity.hpp"
#include "globe.hpp"
#include <vector>

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
        renderer::GlobalProperties _properties;
        std::shared_ptr<spdlog::logger> _log;

        std::unique_ptr<Globe> _globe;

        std::vector<Entity*> _entities;

        void updateGlobalProperties();
    };
}

#endif // ATLAS_SCENE_SCENE_HPP