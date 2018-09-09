#include "Earth.hpp"

namespace atlas
{
    namespace graphics
    {
        Earth::Earth(Renderer* renderer)
        {
            SurfaceTile* root = new SurfaceTile(renderer, 0, 0, 0);

            add_child(root);
        }

        Earth::~Earth()
        {
            // TODO
        }
    }
}