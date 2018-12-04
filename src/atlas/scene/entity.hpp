#ifndef ATLAS_SCENE_ENTITY_HPP
#define ATLAS_SCENE_ENTITY_HPP

#include "transform.hpp"
#include "atlas/renderer/material.hpp"
#include "atlas/renderer/mesh.hpp"
#include "atlas/renderer/entity_data.hpp"

namespace atlas::scene
{
    class Entity
    {
    public:
        Entity();
        Entity(Entity& rhs);
        ~Entity();

        std::string name;
        Transform transform;
        renderer::Material* material;
        std::shared_ptr<renderer::Mesh> mesh;

        void update();

        inline renderer::EntityData* data() const noexcept { return _data.get(); }

    private:
        std::unique_ptr<renderer::EntityData> _data;
    };
}

#endif