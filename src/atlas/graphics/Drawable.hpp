#ifndef ATLAS_DRAWABLE_HPP
#define ATLAS_DRAWABLE_HPP

#include "AtlasGraphics.hpp"
#include "Node.hpp"
#include "Renderer.hpp"
#include "Shader.hpp"
#include "MeshObject.hpp"
#include "Image.hpp"
#include "primitives/Tile.hpp"

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