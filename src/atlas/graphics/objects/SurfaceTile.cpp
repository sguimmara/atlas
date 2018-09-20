#include "SurfaceTile.hpp"
#include "atlas/core/Math.hpp"

const uint32_t TileSubdivisions = 16;

namespace atlas
{
    namespace graphics
    {
        SurfaceTile::SurfaceTile(Ellipsoid ellipsoid, int level, int row, int col) :
            _level(level),
            _row(row),
            _col(col),
            _ellipsoid(ellipsoid),
            Mesh((TileSubdivisions + 1) * (TileSubdivisions + 1))
        {
            int maxLevel = 10;
            float l = 1.0f / maxLevel * level;
            _color = glm::vec3(l, 1.0f - l, 0);

            double h = 0; // height;

            double u, v;

            int n = 1 << level;
            double xSize = 2 * PI / n;
            double ySize = PI / n;

            double x0 = col * xSize - PI;
            double y0 = row * ySize - PI / 2;

            glm::vec2 min(x0, y0);
            glm::vec2 max(x0 + xSize, y0 + ySize);

            const uint16_t subdivX = TileSubdivisions;
            const uint16_t subdivY = TileSubdivisions;

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

            int i = 0;
            for (uint16_t row = 0; row < vertexPerRow; ++row)
            {
                uint16_t jww = row * vertexPerCol;
                uint16_t j1ww = (row + 1) * vertexPerCol;

                for (uint16_t col = 0; col < vertexPerCol; ++col)
                {
                    double lat = min.y + row * yStep;
                    double lon = min.x + col * xStep;

                    positions[i] = Math::LatLonToECEF(lat, lon, ellipsoid);
                    normals[i] = positions[i];

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
            Apply();

            topology = vk::PrimitiveTopology::eTriangleList;
            _material = Material(
                std::vector<Semantic>{Semantic::Position, Semantic::Normal, Semantic::TexCoord},
                std::vector<Descriptor>(),
                Shader::Get("tile.vert"),
                Shader::Get("tile.frag"),
                topology);
        }

        SurfaceTile::~SurfaceTile()
        {
            // TODO
            spdlog::get("renderer")->warn("SurfaceTile: destructor not implemented");
        }

        void SurfaceTile::Update(UpdateContext ctx)
        {
            glm::vec3 camPos = (ctx.camera)->transform()[3];
            glm::vec3 pos = _transform[3];

            auto const dist = glm::distance(camPos, pos);
        }

        void SurfaceTile::Reduce()
        {
            if (_children.size() == 0)
            {
                return;
            }

            for (auto child : _children)
            {
                static_cast<SurfaceTile*>(child)->Reduce();
            }

            if (_children[0]->child_count() == 0)
            {
                _children.clear();
                _flags |= (int)NodeFlags::Drawable;
            }
        }

        void SurfaceTile::Split()
        {
            if (child_count() > 0)
            {
                for (auto child : _children)
                {
                    static_cast<SurfaceTile*>(child)->Split();
                }
            }
            else
            {
                _flags &= ~(int)NodeFlags::Drawable;

                int newLevel = _level + 1;
                int r2 = 2 * _row;
                int c2 = 2 * _col;

                SurfaceTile* topLeft = new SurfaceTile(_ellipsoid, newLevel, r2, c2);
                SurfaceTile* topRight = new SurfaceTile(_ellipsoid, newLevel, r2, c2 + 1);
                SurfaceTile* bottomRight = new SurfaceTile(_ellipsoid, newLevel, r2 + 1, c2 + 1);
                SurfaceTile* bottomLeft = new SurfaceTile(_ellipsoid, newLevel, r2 + 1, c2);

                add_child(topLeft);
                add_child(topRight);
                add_child(bottomRight);
                add_child(bottomLeft);
            }
        }
    }
}