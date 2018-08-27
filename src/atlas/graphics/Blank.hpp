#ifndef ATLAS_BLANK_HPP
#define ATLAS_BLANK_HPP

#include "AtlasGraphics.hpp"

#include "Drawable.hpp"


namespace atlas
{
    namespace graphics
    {
        class Renderer;

        class Blank : public Drawable
        {
        public:
            Blank(Renderer* renderer) : Drawable(renderer) {}
        };
    }
}

#endif