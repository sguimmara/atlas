#ifndef ATLAS_SCENE_GLOBALPROPERTIES_HPP
#define ATLAS_SCENE_GLOBALPROPERTIES_HPP

#include "common.hpp"

namespace atlas::renderer
{
    /* Defines global properties that remain constant accross the frame */
    struct GlobalProperties
    {
        glm::mat4 viewMatrix;
        glm::mat4 projMatrix;
        glm::vec4 sunDirection;         // xyz = direction, w = unused
        glm::vec4 sunColorAndIntensity; // xyz = color, w = intensity
        glm::vec4 viewportTime;         // xy = (width, height), z = elapsed seconds since startup, w = unused
    };
}

#endif