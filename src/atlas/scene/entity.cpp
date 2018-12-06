#include "entity.hpp"
#include "meshbuilder.hpp"
#include "atlas/renderer/pipeline.hpp"

using namespace atlas::scene;
using namespace atlas::renderer;

Entity::Entity()
{}

Entity::Entity(std::shared_ptr<Material> material, std::shared_ptr<Mesh> mesh) :
    _material(material),
    _mesh(mesh),
    _uniformBuffer(std::make_unique<UniformBuffer>(sizeof(EntityProperties), 1, Pipeline::entityPropertyLayout()))
{
    init();
}

Entity::Entity(Entity& rhs) :
    _mesh(rhs._mesh),
    _material(rhs._material),
    _transform(rhs._transform),
    _properties(rhs._properties),
    _uniformBuffer(std::make_unique<UniformBuffer>(sizeof(EntityProperties), 1, Pipeline::entityPropertyLayout()))
{
    init();
}

void atlas::scene::Entity::init()
{
    _uniformBuffer->update(&_properties);
}

Entity::~Entity()
{}

void Entity::update()
{
    EntityProperties props = {};
    props.modelMatrix = _transform.modelMatrix();

    _uniformBuffer->update(&props);
}

std::shared_ptr<Entity> Entity::createDebugEntity(Entity& entity)
{
    auto pipeline = Pipeline::get("debug");
    std::shared_ptr<Material> debugMaterial;
    if (pipeline)
    {
        debugMaterial = std::make_shared<Material>(pipeline);
    }
    else
    {
        debugMaterial = std::make_shared<Material>(R"%(
        {
            "name": "debug",
            "vertex": "default.vert.spv",
            "fragment": "debug.frag.spv",
            "assemblyState": {
                "topology": "lineList"
            },
            "rasterizer": {
                "frontFace": "ccw",
                "polygonMode": "line"
            }
        })%");
    }

    return std::make_shared<Entity>(debugMaterial, MeshBuilder::bounds(entity._mesh->bounds()));
}
