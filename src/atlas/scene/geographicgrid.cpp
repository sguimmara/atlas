#include "geographicgrid.hpp"
#include "meshbuilder.hpp"
#include "atlas/core/srs/ellipsoid.hpp"

using namespace atlas::scene;

GeographicGrid::GeographicGrid(SpatialReference* srs) :
    _latitudeInterval(10),
    _longitudeInterval(10),
    _srs(srs),
    Layer("grid")
{
    createEntities();
}

std::vector<const Entity*> GeographicGrid::entities() const
{
    std::vector<const Entity*> result(_parallels.size());
    for (size_t i = 0; i < _parallels.size(); i++)
    {
        result[i] = _parallels[i].get();
    }
    return result;
}

void GeographicGrid::createEntities()
{
    Pipeline::create(R"%(
        {
            "name": "grid",
            "vertex": "default.vert.spv",
            "fragment": "color.frag.spv",
            "assemblyState":{
                "topology": "lineStrip"
            }
        })%");

    auto equator = Material::create("grid");
    equator->properties().color = vec4(1, 0, 0, 1);
    equator->update();

    auto greenwich = Material::create("grid");
    greenwich->properties().color = vec4(0, 1, 0, 1);
    greenwich->update();

    auto tropic = Material::create("grid");
    tropic->properties().color = vec4(51 / 255.0, 179 / 255.0, 1, 1);
    tropic->update();

    auto gridLine = Material::create("grid");
    gridLine->properties().color = vec4(0.5, 0.5, 0.5, 1);
    gridLine->update();

    auto polarCircle = Material::create("grid");
    polarCircle->properties().color = vec4(0.9, 0.9, 1, 1);
    polarCircle->update();

    auto srs = Ellipsoid::unitSphere();

    _parallels.push_back(std::make_unique<Entity>(equator,
        MeshBuilder::parallel(0, srs)));

    _parallels.push_back(std::make_unique<Entity>(tropic,
        MeshBuilder::parallel(Cartographic::dmsToDD(23, 26, 12.6), srs)));

    _parallels.push_back(std::make_unique<Entity>(tropic,
        MeshBuilder::parallel(-Cartographic::dmsToDD(23, 26, 12.6), srs)));

    _parallels.push_back(std::make_unique<Entity>(polarCircle,
        MeshBuilder::parallel(Cartographic::dmsToDD(66, 33, 47.4), srs)));

    _parallels.push_back(std::make_unique<Entity>(polarCircle,
        MeshBuilder::parallel(-Cartographic::dmsToDD(66, 33, 47.4), srs)));

    for (int i = -90; i < 90; i += (int)_latitudeInterval)
    {
        if (i == 0)
        {
            _parallels.push_back(std::make_unique<Entity>(gridLine,
                MeshBuilder::parallel((double)i, srs)));
        }

        _parallels.push_back(std::make_unique<Entity>(gridLine,
            MeshBuilder::parallel((double)i, srs)));
    }

    for (int i = -180; i < 180; i += (int)_longitudeInterval)
    {
        if (i == 0)
        {
            _parallels.push_back(std::make_unique<Entity>(greenwich,
                MeshBuilder::meridian((double)i, srs)));
        }
        else
        {
            _parallels.push_back(std::make_unique<Entity>(gridLine,
                MeshBuilder::meridian((double)i, srs)));
        }

    }
}
