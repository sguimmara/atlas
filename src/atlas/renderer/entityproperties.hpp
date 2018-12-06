#ifndef ATLAS_SCENE_ENTITYPROPERTIES_HPP
#define ATLAS_SCENE_ENTITYPROPERTIES_HPP

#include "common.hpp"

namespace atlas::renderer
{
    /* Defines entity properties to pass to shaders */
    class EntityProperties
    {
    public:
        glm::mat4 modelMatrix;

        EntityProperties() :
            modelMatrix(1.0)
        {}
    };
}

#endif // ENTITYPROPERTIES