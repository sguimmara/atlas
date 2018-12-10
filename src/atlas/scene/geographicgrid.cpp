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

    // TODO
    auto equator = Material::create("grid");
    equator->properties().color = vec4(1, 0, 0, 1);
    equator->update();

    auto tropic = Material::create("grid");
    tropic->properties().color = vec4(51 / 255.0, 179 / 255.0, 1, 1);
    tropic->update();

    auto parallel = Material::create("grid");
    parallel->properties().color = vec4(0.5, 0.5, 0.5, 1);
    parallel->update();

    // TODO replace with proper height management in referential
    auto srs = Ellipsoid::spherical(1.05);

    _parallels.push_back(std::make_unique<Entity>(equator,
        MeshBuilder::parallel(0, srs)));

    _parallels.push_back(std::make_unique<Entity>(tropic,
        MeshBuilder::parallel(23.26, srs)));

    _parallels.push_back(std::make_unique<Entity>(tropic,
        MeshBuilder::parallel(-23.26, srs)));

    for (int i = -90; i < 90; i += (int)_latitudeInterval)
    {
        if (i == 0)
        {
            continue;
        }

        _parallels.push_back(std::make_unique<Entity>(parallel,
            MeshBuilder::parallel((double)i, srs)));
    }
}
