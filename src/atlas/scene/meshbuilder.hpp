#ifndef ATLAS_SCENE_MESHBUILDER_HPP
#define ATLAS_SCENE_MESHBUILDER_HPP

#include "common.hpp"
#include "atlas/core/srs/spatialreference.hpp"
#include "atlas/core/region.hpp"
#include "atlas/core/bounds.hpp"
#include "atlas/renderer/mesh.hpp"

namespace atlas::scene
{
    using namespace atlas::core;
    using namespace atlas::core::srs;

    class MeshBuilder
    {
    public:
        static std::shared_ptr<renderer::Mesh> terrain(const core::Region&, uint16_t subdivs, const SpatialReference&);
        static std::shared_ptr<renderer::Mesh> bounds(const core::Bounds&);
        static std::shared_ptr<renderer::Mesh> meridian(double longitude, const SpatialReference&);
        static std::shared_ptr<renderer::Mesh> parallel(double latitude, const SpatialReference&);
        static std::shared_ptr<renderer::Mesh> tripodPyramid();
    };
}

#endif