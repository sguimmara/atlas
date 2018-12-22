#pragma once
#ifndef ATLAS_SCENE_TRANSFORM_HPP
#define ATLAS_SCENE_TRANSFORM_HPP

#include "common.hpp"
#include <glm/gtx/matrix_decompose.hpp>

namespace atlas::scene
{
    class Transform
    {
    public:
        Transform();

        inline vec3 position() const noexcept
        {
            vec3 p, s, skew;
            quat o;
            vec4 persp;
            glm::decompose(_modelMatrix, s, o, p, skew, persp);

            return p;
        }

        void move(float x, float y, float z);

        inline glm::mat4 modelMatrix() const noexcept { return _modelMatrix; }

    private:
        glm::mat4 _modelMatrix;
    };
}

#endif