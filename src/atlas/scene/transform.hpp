#ifndef ATLAS_SCENE_TRANSFORM_HPP
#define ATLAS_SCENE_TRANSFORM_HPP

#include "common.hpp"

namespace atlas::scene
{
    class Transform
    {
    public:
        Transform();

        void move(float x, float y, float z);

        inline glm::mat4 modelMatrix() const noexcept { return _modelMatrix; }

    private:
        glm::mat4 _modelMatrix;
    };
}

#endif