#include "globe.hpp"
#include "meshbuilder.hpp"
#include "atlas/renderer/texture.hpp"
#include "atlas/io/fileimagesource.hpp"

using namespace atlas::scene;
using namespace atlas::core;
using namespace atlas::renderer;

Globe::Globe()
{}

Globe::Globe(Ellipsoid ellipsoid) :
    _ellipsoid(ellipsoid),
    _quadtree(std::make_unique<Quadtree>(Region::world(), 8, 4))
{
    // TODO: inject
    _imageSource = std::make_unique<FileImageSource>(Region::world(), "C:/Users/sguimmara/Pictures/dog.jpg");

    if (!Pipeline::get("terrain"))
    {
        Pipeline::create(R"%(
        {
            "name": "terrain",
            "vertex": "default.vert.spv",
            "fragment": "terrain.frag.spv",
            "rasterizer": {
                "frontFace": "ccw"
            }
        })%");
    }
}

void Globe::update()
{
    for (auto& node : *_quadtree)
    {
        if (node.isleaf())
        {
            // first, create the tile if it doesn't exist
            if (_tiles.count(node.key()) == 0)
            {
                auto tile = std::make_shared<Tile>(node.region(), _ellipsoid);

                // request its image
                _imageRequests.push_back(
                    _imageSource->get(Request<Region>(node.region(), tile.get())));

                _tiles.insert({ node.key(), std::move(tile) });
            }
        }
    }

    for (auto& request : _imageRequests)
    {
        if (request.valid())
        {
            auto response = request.get();
            Tile* tile = reinterpret_cast<Tile*>(response.userData());
            tile->setImage(response.data().get());
        }
    }
}

std::vector<const Entity*> Globe::tiles() noexcept
{
    std::vector<const Entity*> result;
    result.reserve(_tiles.size());
    for (auto& t : _tiles)
    {
        result.push_back(t.second->entity());
    }
    return result;
}
