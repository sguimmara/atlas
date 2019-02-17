#include "globe.hpp"
#include "meshbuilder.hpp"
#include "atlas/renderer/texture.hpp"
#include "atlas/io/fileimagesource.hpp"

using namespace atlas::core;
using namespace atlas::core::spatialindex;
using namespace atlas::core::srs;
using namespace atlas::scene;
using namespace atlas::renderer;

Globe::Globe() : Layer("globe")
{}

Globe::Globe(SpatialReference* srs) :
    Layer("globe"),
    _srs(srs),
    _quadtree(std::make_unique<Quadtree>(Region::world(), 2, 1))
{
    _defaultImage = std::make_unique<Image>(8, 8, ImageFormat::RGBA32);

    if (!Pipeline::exists("terrain"))
    {
        Pipeline::create(R"%(
        {
            "name": "terrain",
            "vertex": "default.vert.spv",
            "fragment": "terrain.frag.spv"
        })%");
    }
}

void Globe::setImageSource(ImageSource* src)
{
    _imageRequests.clear();
    _imageSource = src;
}

void Globe::update()
{
    updateQuadtree();

    processPendingImages();
}

std::vector<const Entity*> Globe::entities() const
{
    std::vector<const Entity*> result;
    result.reserve(_tiles.size());
    for (auto& t : _tiles)
    {
        if (t.second->visible())
        {
            result.push_back(t.second->entity());
        }
    }
    return result;
}

std::vector<const Entity*> Globe::debugEntities() const
{
    std::vector<const Entity*> result;
    result.reserve(_tiles.size());
    for (auto& t : _tiles)
    {
        if (t.second->visible())
        {
            result.push_back(t.second->debugEntity());
        }
    }
    return result;
}

bool arbitraryEvaluator(const QuadtreeNode& node)
{
    if (node.key().depth() < 2)
    {
        return true;
    }
    //else if (node.key().depth() < 4)
    //{
    //    return std::rand() % 3 == 0;
    //}
    return false;
}

void Globe::updateQuadtree()
{
    _quadtree->evaluate(arbitraryEvaluator);

    auto list = std::vector<QuadtreeNode>();

    for (auto const& t : _tiles)
    {
        t.second->setVisible(false);
    }

    for (auto& node : *_quadtree)
    {
        if (node.isleaf())
        {
            list.push_back(node);
            // first, create the tile if it doesn't exist
            if (_tiles.count(node.key()) == 0)
            {
                auto r = node.region();

                auto tile = std::make_shared<Tile>(r, *_srs);

                tile->setImage(_defaultImage.get());

                // request its image
                _imageRequests.push_back(
                    _imageSource->get(Request<Region>(r, tile.get())));

                _tiles.insert({ node.key(), std::move(tile) });
            }

            _tiles[node.key()]->setVisible(true);
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
