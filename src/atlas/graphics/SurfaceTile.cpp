#include "SurfaceTile.hpp"
#include "Camera.hpp"

namespace atlas
{
    namespace graphics
    {
        SurfaceTile::SurfaceTile(int level, int row, int col) :
            _level(level),
            _row(row),
            _col(col),
            Mesh(primitives::Tile::Create(16, level, row, col), Material())
        {
            //_material = 
            int maxLevel = 10;
            float l = 1.0f / maxLevel * level;
            _color = glm::vec3(l, 1.0f - l, 0);
        }

        SurfaceTile::~SurfaceTile()
        {
            // TODO
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

                SurfaceTile* topLeft = new SurfaceTile(newLevel, r2, c2);
                SurfaceTile* topRight = new SurfaceTile(newLevel, r2, c2 + 1);
                SurfaceTile* bottomRight = new SurfaceTile(newLevel, r2 + 1, c2 + 1);
                SurfaceTile* bottomLeft = new SurfaceTile(newLevel, r2 + 1, c2);

                add_child(topLeft);
                add_child(topRight);
                add_child(bottomRight);
                add_child(bottomLeft);
            }
        }
    }
}