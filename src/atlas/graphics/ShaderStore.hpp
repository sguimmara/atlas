#ifndef ATLAS_GRAPHICS_SHADERSTORE_HPP
#define ATLAS_GRAPHICS_SHADERSTORE_HPP

#include "AtlasGraphics.hpp"
#include "Shader.hpp"

namespace atlas
{
    namespace graphics
    {
        class ShaderStore
        {
            static Init(std::string shaderDirectory);
        };
    }
}

#endif