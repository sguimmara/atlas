#ifndef ATLAS_SCENE_ENTITY_HPP
#define ATLAS_SCENE_ENTITY_HPP

#include "transform.hpp"
#include "atlas/renderer/material.hpp"
#include "atlas/renderer/mesh.hpp"
#include "atlas/renderer/uniformbuffer.hpp"
#include "atlas/renderer/entityproperties.hpp"

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

        inline renderer::UniformBuffer* data() const noexcept { return _uniformBuffer.get(); }

    private:
        renderer::EntityProperties _properties;
        std::unique_ptr<renderer::UniformBuffer> _uniformBuffer;
    };
}

#endif