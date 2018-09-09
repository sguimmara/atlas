#ifndef ATLAS_GRAPHICS_SURFACETILE_HPP
#define ATLAS_GRAPHICS_SURFACETILE_HPP

#include "AtlasGraphics.hpp"
#include "Node.hpp"
#include "Drawable.hpp"

namespace atlas
{
    namespace graphics
    {
        /**
        * @brief      The SurfaceTile node represents a fragment of the earth's surface.
        */
        class SurfaceTile : public Drawable
        {
        public:
            SurfaceTile(Renderer* renderer, int level, int row, int col);
            ~SurfaceTile();

            void Update(UpdateContext ctx);

            void Split();

        private:
            int _level;
            int _row;
            int _col;
        };
    }
}

#endif