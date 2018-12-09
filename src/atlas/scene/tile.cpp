#include "tile.hpp"
#include "meshbuilder.hpp"

using namespace atlas::scene;

Tile::Tile(Region region, const SpatialReference& srs) : _region(region)
{
    _entity = std::make_unique<Entity>(
        std::make_shared<Material>(Pipeline::get("terrain")),
        MeshBuilder::terrain(region, 32, srs));
}

void Tile::setImage(const Image* image)
{
    _image = std::make_unique<Texture>(*image);

    _entity->material().setTexture("image", _image.get());
}
