#include "tile.hpp"
#include "meshbuilder.hpp"

using namespace atlas::scene;

Tile::Tile(Region region, const SpatialReference& srs) :
    _visible(false),
    _region(region)
{
    _entity = std::make_unique<Entity>(
        std::make_shared<Material>(Pipeline::get("terrain")),
        MeshBuilder::terrain(region, 16, srs));

    _debugEntity = std::make_unique<Entity>(
        std::make_shared<Material>(Pipeline::get("debug")),
        MeshBuilder::region(region.raise(0.01), srs));
}

void Tile::setImage(const Image* image)
{
    _image = std::make_unique<Texture>(*image);

    _entity->material().setTexture("image", _image.get());
}
