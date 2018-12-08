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
    _defaultImage = std::make_unique<Image>(8, 8, ImageFormat::RGBA32);

    // TODO: inject
    _imageSource = std::make_unique<FileImageSource>(Region::world(), "C:/Users/sguimmara/Documents/work/c++/atlas/images/blue_marble.jpg");

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
    updateQuadtree();

    processPendingImages();
}

void Globe::updateQuadtree()
{
    for (auto& node : *_quadtree)
    {
        if (node.isleaf())
        {
            // first, create the tile if it doesn't exist
            if (_tiles.count(node.key()) == 0)
            {
                auto k = node.key();
                auto r = node.region();

                auto tile = std::make_shared<Tile>(node.region(), _ellipsoid);

                tile->setImage(_defaultImage.get());

                // request its image
                _imageRequests.push_back(
                    _imageSource->get(Request<Region>(node.region(), tile.get())));

                _tiles.insert({ node.key(), std::move(tile) });
            }
        }
    }
}

void Globe::processPendingImages()
{
    bool canClear = true;
    for (auto& request : _imageRequests)
    {
        if (request.valid() && (request.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready))
        {
            auto response = request.get();
            Tile* tile = reinterpret_cast<Tile*>(response.userData());
            tile->setImage(response.data().get());
        }
        else
        {
            canClear = false;
        }
    }

    if (canClear)
    {
        _imageRequests.clear();
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