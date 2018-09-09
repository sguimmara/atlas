#ifndef ATLAS_GRAPHICS_RENDERINGOPTIONS_HPP
#define ATLAS_GRAPHICS_RENDERINGOPTIONS_HPP

#include "AtlasGraphics.hpp"

namespace atlas
{
    namespace graphics
    {
        /**
        * @brief      Set of static options to change graphics objects at runtime.
        */
        class RenderingOptions
        {
        public:
            static vk::PolygonMode PolygonMode;
        };
    }
}

#endif