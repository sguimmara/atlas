#include "entity.hpp"
#include "atlas/renderer/pipeline.hpp"

using namespace atlas::scene;
using namespace atlas::renderer;

Entity::Entity() :
    _uniformBuffer(std::make_unique<renderer::UniformBuffer>(sizeof(EntityProperties), 1, Pipeline::entityPropertyLayout()))
{
    _uniformBuffer->update(&_properties);
}

Entity::Entity(Entity& rhs) :
    mesh(rhs.mesh),
    material(rhs.material),
    transform(rhs.transform),
    _properties(rhs._properties),
    _uniformBuffer(std::make_unique<renderer::UniformBuffer>(sizeof(EntityProperties), 1, Pipeline::entityPropertyLayout()))
{
    _uniformBuffer->update(&_properties);
}

Entity::~Entity()
{
}

void Entity::update()
{
    renderer::EntityProperties props = {};
    props.modelMatrix = transform.modelMatrix();

    _uniformBuffer->update(&props);
}
