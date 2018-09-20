#include "Earth.hpp"
#include "atlas/graphics/Mesh.hpp"
#include "atlas/core/Color.hpp"
#include "atlas/core/Math.hpp"

using namespace atlas::graphics::objects;

Earth::Earth(Ellipsoid ellipsoid, double scale) :
    _ellipsoid(ellipsoid),
    _scale(scale)
{
    const float VulkanToEcef[] = {
        0, 0,-1, 0,
        1, 0, 0, 0,
        0,-1, 0, 0,
        0, 0, 0, 1
    };

    setLocalTransform(make_mat4(VulkanToEcef));

    // equatorial plane
    Mesh* plane = Mesh::MakePlane(Color::gray);
    plane->setLocalTransform(glm::scale(plane->localTransform(), vec3(10, 10, 10)));
    add_child(plane);

    // world origin
    add_child(Mesh::MakePoint(Color::white, vec3(0, 0, 0)));

    // axes
    add_child(Mesh::MakeLine(Color::red, vec3(0, 0, 0), vec3(5, 0, 0)));
    add_child(Mesh::MakeLine(Color::green, vec3(0, 0, 0), vec3(0, 5, 0)));
    add_child(Mesh::MakeLine(Color::blue, vec3(0, 0, 0), vec3(0, 0, 5)));

    // greenwich meridian
    add_child(Mesh::MakeMeridian(Color::green, 0, ellipsoid));

    // equator and tropics
    add_child(Mesh::MakeParallel(Color::cyan, 0, ellipsoid));
    add_child(Mesh::MakeParallel(Color::red, Math::ToRadians(23.43686), ellipsoid));
    add_child(Mesh::MakeParallel(Color::red, Math::ToRadians(-23.43686), ellipsoid));

    // earth shape : solid and grid
    add_child(Mesh::MakeSolidEllipsoid(Color::gray, 32, ellipsoid));
    add_child(Mesh::MakeEllipsoid(Color::white, 20, ellipsoid));
}

Earth::~Earth()
{
    // TODO
}