#include "entity.hpp"

using namespace atlas::scene;

Entity::Entity() :
    _data(std::make_unique<renderer::EntityData>())
{
    _data->update(renderer::EntityProperties());
}

Entity::Entity(Entity& rhs) :
    mesh(rhs.mesh),
    material(rhs.material),
    transform(rhs.transform),
    _data(std::make_unique<renderer::EntityData>())
{}


Entity::~Entity()
{
}

void Entity::update()
{
    renderer::EntityProperties props = {};
    props.modelMatrix = transform.modelMatrix();

    _data->update(props);
}
