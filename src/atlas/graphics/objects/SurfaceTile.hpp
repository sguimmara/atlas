#ifndef ATLAS_GRAPHICS_SURFACETILE_HPP
#define ATLAS_GRAPHICS_SURFACETILE_HPP

#include "atlas/graphics/AtlasGraphics.hpp"
#include "atlas/graphics/Node.hpp"
#include "atlas/graphics/Mesh.hpp"

namespace atlas
{
    namespace graphics
    {
        /**
        * @brief      The SurfaceTile node represents a fragment of the earth's surface.
                      It is part of a tile matrix where tiles is subdivided into 4 sub tiles.
        */
        class SurfaceTile : public Mesh
        {
        public:
            SurfaceTile(Ellipsoid ellipsoid, int level, int row, int col);
            ~SurfaceTile();

            void Update(UpdateContext ctx);

            void Split();
            void Reduce();

        private:
            Ellipsoid _ellipsoid;
            int _level;
            int _row;
            int _col;
        };
    }
}

#endif