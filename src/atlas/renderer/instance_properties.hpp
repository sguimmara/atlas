#ifndef ATLAS_SCENE_INSTANCEPROPERTIES_HPP
#define ATLAS_SCENE_INSTANCEPROPERTIES_HPP

#include "common.hpp"

namespace atlas::renderer
{
    /* Defines per instance properties */
    struct InstanceProperties
    {
        glm::mat4 modelMatrix;

        InstanceProperties() :
            modelMatrix(1.0)
        {}
    };
}

#endif