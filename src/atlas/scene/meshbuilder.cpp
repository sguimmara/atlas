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
