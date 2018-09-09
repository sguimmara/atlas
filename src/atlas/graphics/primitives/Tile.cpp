#include "Tile.hpp"
#include <cmath>

namespace agp = atlas::graphics::primitives;

namespace atlas
{
    namespace graphics
    {
        namespace primitives
        {
            Tile::Tile(vk::PhysicalDevice gpu, vk::Device device, uint16_t subdivs, glm::vec2 min, glm::vec2 max) :
                Mesh((subdivs * 2 + 1) * (subdivs + 1))
            {
                const double a = 6378137;     // semimajor axis
                const double b = 6356752.314; // semiminor axis
                const double e2 = 1 - (b * b) / (a * a);

                double h = 0; // height;

                double u, v;
                double x, y, z;

                const uint16_t subdivX = subdivs * 2;
                const uint16_t subdivY = subdivs;

                const double xStep = (max.x - min.x) / subdivX;
                const double yStep = (max.y - min.y) / subdivY;

                const uint16_t vertexPerRow = subdivY + 1;
                const uint16_t vertexPerCol = subdivX + 1;
                const int size = vertexPerRow * vertexPerCol;

                std::vector<glm::vec3> positions;
                std::vector<glm::vec3> normals;
                std::vector<glm::vec2> uv;
                std::vector<uint16_t> indices;

                positions.resize(size);
                normals.resize(size);
                uv.resize(size);

                float scale = 0.0000001f;

                int i = 0;

                for (uint16_t row = 0; row < vertexPerRow; ++row)
                {
                    uint16_t jww = row * vertexPerCol;
                    uint16_t j1ww = (row + 1) * vertexPerCol;

                    for (uint16_t col = 0; col < vertexPerCol; ++col)
                    {
                        double lat = min.y + row * yStep;
                        double lon = min.x + col * xStep;

                        double nLat = a / std::sqrt(1 - e2 * std::sin(lat));

                        x = (nLat + h) * std::cos(lat) * std::cos(lon);
                        y = (nLat + h) * std::cos(lat) * std::sin(lon);
                        z = (((b*b) / (a*a)*nLat) + h) * std::sin(lat);

                        u = col / static_cast<double>(subdivX);
                        v = row / static_cast<double>(subdivY);

                        positions[i] = glm::vec3{ x, z, y } * scale;
                        normals[i] = glm::normalize(glm::vec3{ x, z, y });
                        uv[i] = { u, v };
                        ++i;

                        if (row < subdivY && col < subdivX)
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

                SetPositions(positions);
                SetNormals(normals);
                SetUV(uv);
                SetIndices(gpu, device, indices);
                Apply(gpu, device);
            }

            Tile Tile::Create(atlas::graphics::Renderer * renderer, uint16_t subdivs, int level, int row, int col)
            {
                int n = 1 << level;
                double xSize = 2 * PI / n;
                double ySize = PI / n;

                double x = col * xSize - PI;
                double y = row * ySize - PI / 2;

                glm::vec2 min(x, y);
                glm::vec2 max(x + xSize, y + ySize);

                return Tile(renderer->gpu(), renderer->device(), subdivs, min, max);
            }
        }
    }
}
