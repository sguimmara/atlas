#pragma once
#ifndef ATLAS_RENDERER_VERTEX_HPP
#define ATLAS_RENDERER_VERTEX_HPP

#include "common.hpp"

namespace atlas::renderer
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 st;
    };
}

#endif // ATLAS_RENDERER_VERTEX_HPP