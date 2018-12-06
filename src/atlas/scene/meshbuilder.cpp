#include "meshbuilder.hpp"
#include "atlas/renderer/vertex.hpp"

using namespace atlas::core;
using namespace atlas::scene;
using namespace atlas::renderer;

std::shared_ptr<Mesh> MeshBuilder::terrain(const Region& region, uint16_t subdivs, const Ellipsoid& ellipsoid)
{
    if (subdivs < 2)
    {
        throw std::runtime_error("invalid subdivs. must be at least 2");
    }
    const double xStep = region.width() / (subdivs - 1);
    const double yStep = region.height() / (subdivs - 1);

    const auto stStep = 1.0f / (subdivs - 1);

    const uint16_t size = (uint16_t)(subdivs * subdivs);

    auto vertices = std::vector<Vertex>();
    auto indices = std::vector<uint16_t>();
    vertices.reserve(size);

    for (uint16_t row = 0; row < subdivs; ++row)
    {
        uint16_t jww = row * subdivs;
        uint16_t j1ww = (row + 1) * subdivs;

        for (uint16_t col = 0; col < subdivs; ++col)
        {
            Vertex v;

            double lat = region.south() + row * yStep;
            double lon = region.west() + col * xStep;

            v.position = ellipsoid.position(lat, lon, 0);
            v.normal = ellipsoid.normal(lat, lon);
            v.st = vec2(1 - (col * stStep), row * stStep);

            vertices.push_back(v);

            if (row < (subdivs - 1) && col < (subdivs - 1))
            {
                uint16_t a = col + jww;
                uint16_t b = (col + 1) + jww;
                uint16_t c = (col + 1) + j1ww;
                uint16_t d = col + j1ww;

                indices.push_back(a);
                indices.push_back(b);
                indices.push_back(c);

                indices.push_back(c);
                indices.push_back(d);
                indices.push_back(a);
            }
        }
    }

    return std::make_shared<Mesh>(vertices, indices);
}

std::shared_ptr<Mesh> MeshBuilder::bounds(const Bounds& bounds)
{
    const float minX = bounds.min.x;
    const float minY = bounds.min.y;
    const float minZ = bounds.min.z;
    const float maxX = bounds.max.x;
    const float maxY = bounds.max.y;
    const float maxZ = bounds.max.z;

    std::vector<Vertex> vertices
    {
        Vertex{{minX, minY, minZ}},
        Vertex{{minX, maxY, minZ}},
        Vertex{{maxX, maxY, minZ}},
        Vertex{{maxX, minY, minZ}},

        Vertex{{minX, minY, maxZ}},
        Vertex{{minX, maxY, maxZ}},
        Vertex{{maxX, maxY, maxZ}},
        Vertex{{maxX, minY, maxZ}}
    };
    std::vector<uint16_t> indices
    {
        0, 1, 2, 0, 2, 3,
        4, 5, 6, 4, 6, 7,
        1, 5, 4, 1, 4, 0,
        0, 4, 7, 0, 7, 3,
        2, 6, 5, 2, 5, 1,
        3, 7, 6, 3, 6, 2
    };

    return std::make_shared<Mesh>(vertices, indices);
}
