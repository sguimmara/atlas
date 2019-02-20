#pragma once
#ifndef ATLAS_SCENE_ENTITY_HPP
#define ATLAS_SCENE_ENTITY_HPP

#include "transform.hpp"
#include "atlas/renderer/material.hpp"
#include "atlas/renderer/mesh.hpp"
#include "atlas/renderer/uniformbuffer.hpp"
#include "atlas/renderer/entityproperties.hpp"

namespace atlas::scene
{
    using namespace atlas::renderer;

    class Entity
    {
    public:
        Entity();
        Entity(std::shared_ptr<Material>, std::shared_ptr<Mesh>);
        Entity(Entity& rhs);
        void init();
        ~Entity();

        void update();

        inline bool visible() const noexcept { return _visible; }
        inline void setVisible(bool value) noexcept { _visible = value; }
        inline const Transform& transform() const noexcept { return _transform; }
        inline Material& material() const noexcept { return *_material; }
        inline Mesh& mesh() const noexcept { return *_mesh; }
        inline UniformBuffer* data() const noexcept { return _uniformBuffer.get(); }

        static std::shared_ptr<Entity> createDebugEntity(Entity& entity);

    private:

        bool _visible;
        Transform _transform;
        std::shared_ptr<Material> _material;
        std::shared_ptr<Mesh> _mesh;
        EntityProperties _properties;
        std::unique_ptr<UniformBuffer> _uniformBuffer;
    };
}

#endif