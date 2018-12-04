#include "bounds.hpp"

using namespace atlas::core;

Bounds::Bounds() : min(0), max(0)
{}

Bounds::Bounds(const std::vector<atlas::renderer::Vertex>& vertices)
{
    max = vec3(
        std::numeric_limits<float>::min(),
        std::numeric_limits<float>::min(),
        std::numeric_limits<float>::min());

    min = vec3(
        std::numeric_limits<float>::max(),
        std::numeric_limits<float>::max(),
        std::numeric_limits<float>::max());

    for (size_t i = 0; i < vertices.size(); i++)
    {
        auto const p = vertices[i].position;

        max.x = p.x > max.x ? p.x : max.x;
        max.y = p.y > max.y ? p.y : max.y;
        max.z = p.z > max.z ? p.z : max.z;

        min.x = p.x < min.x ? p.x : min.x;
        min.y = p.y < min.y ? p.y : min.y;
        min.z = p.z < min.z ? p.z : min.z;
    }
}
