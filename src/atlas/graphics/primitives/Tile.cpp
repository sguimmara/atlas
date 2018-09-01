#include "Tile.hpp"
#include <cmath>

namespace agp = atlas::graphics::primitives;

agp::Tile::Tile(vk::PhysicalDevice gpu, vk::Device device, uint16_t subdivs, glm::vec2 min, glm::vec2 max)
{
    int w = subdivs;
    int h = subdivs * 2;

    double deltaX = (max[0] - min[0]) / w;
    double deltaY = (max[1] - min[1]) / h;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uv;
    std::vector<uint16_t> indices;

    double x, y, z;
    uint16_t idx = 0;
    double startX = min[0];
    double startY = min[1];
    z = 0;

    for (size_t j = 0; j < h + 1; ++j)
    {
        y = startY + j * deltaY;

        for (size_t i = 0; i < w + 1; ++i)
        {
            x = startX + i * deltaX;
            double x0 = std::sin(x) * std::cos(y);
            double z0 = std::sin(x) * std::sin(y);
            double y0 = std::cos(x);
            uv.push_back({ (float)x, (float)y });

            positions.push_back({ (float)x0, (float)y0, (float)z0 });
            normals.push_back({ (float)x0, (float)y0, (float)z0 });
        }
    }

    uint16_t ww = w + 1;
    
    for (uint16_t j = 0; j < h; ++j)
    {
        uint16_t jww = j * ww;
        uint16_t j1ww = (j + 1) * ww;

        for (uint16_t i = 0; i < w; ++i)
        {
            uint16_t a = i + jww;
            uint16_t b = (i + 1) + jww;
            uint16_t c = (i + 1) + j1ww;
            uint16_t d = i + j1ww;

            indices.push_back(a);
            indices.push_back(b);
            indices.push_back(c);
            
            indices.push_back(c);
            indices.push_back(d);
            indices.push_back(a);

        }
    }

    SetPositions(gpu, device, positions);
    SetNormals(gpu, device, normals);
    SetUV(gpu, device, uv);
    SetIndices(gpu, device, indices);
}
