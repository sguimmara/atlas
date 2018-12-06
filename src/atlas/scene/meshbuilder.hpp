#ifndef ATLAS_SCENE_MESHBUILDER_HPP
#define ATLAS_SCENE_MESHBUILDER_HPP

#include "common.hpp"
#include "atlas/core/ellipsoid.hpp"
#include "atlas/core/region.hpp"
#include "atlas/core/bounds.hpp"
#include "atlas/renderer/mesh.hpp"

namespace atlas::scene
{
    class MeshBuilder
    {
    public:
        static std::shared_ptr<renderer::Mesh> terrain(const core::Region&, uint16_t subdivs, const core::Ellipsoid& ellipsoid);
        static std::shared_ptr<renderer::Mesh> bounds(const core::Bounds&);
    };
}

#endif