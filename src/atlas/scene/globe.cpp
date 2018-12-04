#include "globe.hpp"
#include "meshbuilder.hpp"
#include "atlas/renderer/texture.hpp"

using namespace atlas::scene;
using namespace atlas::renderer;

Globe::Globe() :
    _quadtree(std::make_unique<Quadtree>(Region::world(), 8, 4))
{
    _TEMPterrainMaterial = std::make_unique<Material>(R"%(
        {
            "name": "terrain",
            "vertex": "default.vert.spv",
            "fragment": "default.frag.spv",
            "rasterizer": {
                "frontFace": "ccw"
            }
        })%");

    _TEMPtexture = std::make_unique<Texture>("C:/Users/sguimmara/Documents/work/c++/atlas/images/uv_grid.jpg");
    _TEMPterrainMaterial->setTexture("diffuse", _TEMPtexture.get());
    _TEMPterrainMaterial->setTexture("specular", _TEMPtexture.get());

    for (auto& node : *_quadtree)
    {
        if (node.isleaf())
        {
            auto ent = std::make_shared<Entity>();
            ent->mesh = MeshBuilder::terrain(node.region(), 16, Ellipsoid::unitSphere());
            ent->material = _TEMPterrainMaterial.get();
            _tiles.push_back(ent);
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
