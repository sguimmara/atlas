#ifndef ATLAS_DRAWABLE_HPP
#define ATLAS_DRAWABLE_HPP

#include "AtlasGraphics.hpp"
#include "Node.hpp"

namespace atlas
{
    namespace graphics
    {
        /**
        * @brief A Drawable is a Node that can issue render commands
        */
        class Drawable : public Node
        {
            virtual void Draw(DrawContext context) = 0;
        };
    }
}

#endif