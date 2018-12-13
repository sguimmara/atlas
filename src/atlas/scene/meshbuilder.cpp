#include "meshbuilder.hpp"
#include "atlas/renderer/vertex.hpp"

using namespace atlas::core;
using namespace atlas::core::srs;
using namespace atlas::scene;
using namespace atlas::renderer;

std::shared_ptr<Mesh> MeshBuilder::terrain(const Region& region, uint16_t subdivs, const SpatialReference& srs)
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

            v.position = srs.position(lat, lon, 0);
            v.normal = srs.normal(lat, lon);
            v.st = vec2(col * stStep, row * stStep);

            vertices.push_back(v);

            if (row < (subdivs - 1) && col < (subdivs - 1))
            {
                uint16_t a = col + jww;
                uint16_t b = (col + 1) + jww;
                uint16_t c = (col + 1) + j1ww;
                uint16_t d = col + j1ww;

                indices.push_back(a);
                indices.push_back(c);
                indices.push_back(b);

                indices.push_back(c);
                indices.push_back(a);
                indices.push_back(d);
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

    const auto A = Vertex{ {minX, minY, minZ} };
    const auto B = Vertex{ {minX, maxY, minZ} };
    const auto C = Vertex{ {maxX, maxY, minZ} };
    const auto D = Vertex{ {maxX, minY, minZ} };

    const auto E = Vertex{ {minX, minY, maxZ} };
    const auto F = Vertex{ {minX, maxY, maxZ} };
    const auto G = Vertex{ {maxX, maxY, maxZ} };
    const auto H = Vertex{ {maxX, minY, maxZ} };

    std::vector<Vertex> vertices
    {
        A,B,B,C,C,D,D,A,
        E,F,F,G,G,H,H,E,
        A,E,B,F,C,G,D,H
    };

    std::vector<uint16_t> indices(0);

    return std::make_shared<Mesh>(vertices, indices);
}

std::shared_ptr<Mesh> MeshBuilder::region(const Region& region, const SpatialReference& srs)
{
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;

    size_t subdivs = 6;
    double step = 1.0 / subdivs;

    // north row
    double t = 0;
    for (size_t i = 0; i < subdivs; i++)
    {
        vertices.push_back(Vertex{ srs.position(Cartographic::lerp(region.topLeft(), region.topRight(), t)) });
        t += step;
    }
    // east column
    t = 0;
    for (size_t i = 0; i < subdivs; i++)
    {
        vertices.push_back(Vertex{ srs.position(Cartographic::lerp(region.topRight(), region.bottomRight(), t)) });
        t += step;
    }
    // south row
    t = 0;
    for (size_t i = 0; i < subdivs; i++)
    {
        vertices.push_back(Vertex{ srs.position(Cartographic::lerp(region.bottomRight(), region.bottomLeft(), t)) });
        t += step;
    }
    // west column
    t = 0;
    for (size_t i = 0; i < subdivs; i++)
    {
        vertices.push_back(Vertex{ srs.position(Cartographic::lerp(region.bottomLeft(), region.topLeft(), t)) });
        t += step;
    }

    vertices.push_back(vertices[0]);
    auto result = std::make_shared<Mesh>(vertices, indices);
    result->topology = vk::PrimitiveTopology::eLineStrip;
    return result;
}

std::shared_ptr<Mesh> MeshBuilder::meridian(double longitude, const SpatialReference& srs)
{
    const size_t subdivs = 180;
    std::vector<Vertex> vertices(subdivs + 1);
    std::vector<uint16_t> indices(0);
    for (int i = -90; i < 90; i++)
    {
        Vertex v{ srs.position(Cartographic::fromDegrees((double)i, longitude, 0)) };
        vertices[i+90] = v;
    }
    vertices[subdivs] = vertices[0];

    auto result = std::make_shared<Mesh>(vertices, indices);
    result->topology = vk::PrimitiveTopology::eLineStrip;
    return result;
}

std::shared_ptr<Mesh> MeshBuilder::parallel(double latitude, const SpatialReference& srs)
{
    const size_t subdivs = 360;
    std::vector<Vertex> vertices(subdivs + 1);
    std::vector<uint16_t> indices(0);
    for (size_t i = 0; i < subdivs; i++)
    {
        Vertex v{ srs.position(Cartographic::fromDegrees(latitude, (double)i, 0)) };
        vertices[i] = v;
    }
    vertices[subdivs] = vertices[0];

    auto result = std::make_shared<Mesh>(vertices, indices);
    result->topology = vk::PrimitiveTopology::eLineStrip;
    return result;
}

std::shared_ptr<Mesh> MeshBuilder::tripodPyramid()
{
    std::vector<Vertex> vertices
    {
        Vertex{vec3(0, 0, 0), vec3(1, 1, 1) },
        Vertex{vec3(1, 0, 0), vec3(1, 0, 0) },
        Vertex{vec3(0, 1, 0), vec3(0, 1, 0) },
        Vertex{vec3(0, 0, 1), vec3(0, 0, 1) },
    };

    std::vector<uint16_t> indices{ 3,1,2,3,0,1,1,0,2,3,2,0 };

    return std::make_shared<Mesh>(vertices, indices);
}
