#pragma once
#ifndef ATLAS_CORE_BOUNDS_HPP
#define ATLAS_CORE_BOUNDS_HPP

#include "common.hpp"
#include "atlas/renderer/vertex.hpp"

namespace atlas::core
{
    // An axis aligned bounding box (AABB).
    struct Bounds
    {
        vec3 min;
        vec3 max;

        // constructs zero bounds (ie without volume).
        Bounds();

        // constructs a bounds from the given vertices.
        Bounds(const std::vector<renderer::Vertex>& vertices);
    };
}

#endif // ATLAS_CORE_BOUNDS_HPP