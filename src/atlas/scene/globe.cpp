#include "globe.hpp"
#include "meshbuilder.hpp"
#include "atlas/renderer/texture.hpp"

using namespace atlas::scene;
using namespace atlas::renderer;

Globe::Globe() :
    _quadtree(std::make_unique<Quadtree>(Region::world(), 8, 4))
{
    _TEMPterrainMaterial = std::make_shared<Material>(R"%(
        {
            "name": "terrain",
            "vertex": "default.vert.spv",
            "fragment": "default.frag.spv",
            "rasterizer": {
                "frontFace": "ccw"
            }
        })%");

    // TODO cleanup material allocations.
    _TEMPtexture = std::make_unique<Texture>("C:/Users/sguimmara/Documents/work/c++/atlas/images/uv_grid.jpg");
    _TEMPterrainMaterial->setTexture("diffuse", _TEMPtexture.get());
    _TEMPterrainMaterial->setTexture("specular", _TEMPtexture.get());

    // TODO allocate tiles dynamically after each quadtree update.
    // 1. nodes that became leaves : allocate tile
    // 2. nodes that became non-leaves : hide them, keep them for a while as cache
    for (auto& node : *_quadtree)
    {
        if (node.isleaf())
        {
            auto tile = std::make_shared<Entity>(
                _TEMPterrainMaterial,
                MeshBuilder::terrain(node.region(), 16, Ellipsoid::unitSphere()));
            _tiles.push_back(tile);
            _tiles.push_back(Entity::createDebugEntity(*tile));
        }
    }
}

std::vector<Entity*> Globe::tiles() noexcept
{
    std::vector<Entity*> result;
    result.reserve(_tiles.size());
    for (auto& t : _tiles)
    {
        result.push_back(t.get());
    }
    return result;
}
