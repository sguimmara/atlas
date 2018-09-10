#include "Tile.hpp"
#include <cmath>
#include <random>

namespace agp = atlas::graphics::primitives;

namespace atlas
{
    namespace graphics
    {
        namespace primitives
        {
            Tile::Tile(uint16_t subdivs, glm::vec2 min, glm::vec2 max) :
                MeshObject((subdivs + 1) * (subdivs + 1))
            {
                const double a = 6378137;     // semimajor axis
                const double b = 6356752.314; // semiminor axis
                const double e2 = 1 - (b * b) / (a * a);

                double h = 0; // height;

                double u, v;
                double x, y, z;

                const uint16_t subdivX = subdivs;
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

                // TEMPORARY
                std::vector<double> elevs;
                elevs.resize(size);
                for (uint16_t row = 0; row < vertexPerRow; ++row)
                {
                    double yy = 2 * (row / static_cast<double>(vertexPerRow - 1) - 0.5);

                    for (uint16_t col = 0; col < vertexPerCol; ++col)
                    {
                        double xx = 2 * (col / static_cast<double>(vertexPerCol - 1) - 0.5);
                        double zz = 1 - std::abs(xx + yy) - std::abs(yy - xx);
                        double hh = zz / 2.0 + 0.5;

                        elevs[col + row * vertexPerRow] = hh * 1000000;
                    }
                }

                for (uint16_t row = 0; row < vertexPerRow; ++row)
                {
                    uint16_t jww = row * vertexPerCol;
                    uint16_t j1ww = (row + 1) * vertexPerCol;

                    for (uint16_t col = 0; col < vertexPerCol; ++col)
                    {
                        double lat = min.y + row * yStep;
                        double lon = min.x + col * xStep;

                        double sinlat = std::sin(lat);
                        double sinlon = std::sin(lon);
                        double coslon = std::cos(lon);
                        double coslat = std::cos(lat);

                        double nLat = a / std::sqrt(1 - e2 * sinlat);

                        h = elevs[col + row * vertexPerRow];

                        x = (nLat + h) * coslat * coslon;
                        y = (nLat + h) * coslat * sinlon;
                        z = (((b*b) / (a*a)*nLat) + h) * sinlat;

                        positions[i] = glm::vec3{ x, z, y } *scale;

                        float hL = static_cast<float>(elevs[std::max(col - 1, vertexPerCol - 1) + row * vertexPerRow]);
                        float hR = static_cast<float>(elevs[std::min(col + 1, vertexPerCol - 1) + row * vertexPerRow]);
                        float hU = static_cast<float>(elevs[col + std::max(row - 1, vertexPerRow - 1) * vertexPerRow]);
                        float hD = static_cast<float>(elevs[col + std::min(row + 1, vertexPerRow - 1) * vertexPerRow]);

                        glm::vec3 normal = glm::normalize(glm::vec3{ hL - hR, hD - hU, 2.0 });

                        double up[9] =
                        {
                            -sinlat * coslon, -sinlat * sinlon, coslon,
                            -sinlon, coslon, 0,
                            -coslat * coslon, -coslat * sinlon, -sinlat,
                        };

                        float upF[9]
                        {
                            (float)up[0], (float)up[3], (float)up[6],
                            (float)up[1], (float)up[4], (float)up[7],
                            (float)up[2], (float)up[5], (float)up[8],
                        };

                        glm::mat3 localUp = glm::make_mat3(upF);

                        normals[i] = localUp * normal;
                        normals[i] = normal;

                        /* texture coordinates */
                        u = col / static_cast<double>(subdivX);
                        v = row / static_cast<double>(subdivY);
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
                SetIndices(indices);
                topology = vk::PrimitiveTopology::eTriangleList;
                Apply();
            }

            Tile Tile::Create(uint16_t subdivs, int level, int row, int col)
            {
                int n = 1 << level;
                double xSize = 2 * PI / n;
                double ySize = PI / n;

                double x = col * xSize - PI;
                double y = row * ySize - PI / 2;

                glm::vec2 min(x, y);
                glm::vec2 max(x + xSize, y + ySize);

                return Tile(subdivs, min, max);
            }
        }
    }
}
