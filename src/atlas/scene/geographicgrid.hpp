#ifndef ATLAS_SCENE_GEOGRAPHICGRID_HPP
#define ATLAS_SCENE_GEOGRAPHICGRID_HPP

#include "layer.hpp"
#include "atlas/core/srs/spatialreference.hpp"

namespace atlas::scene
{
    using namespace atlas::core::srs;

    // Draws a geographic grid (parallels, meridians, polar circles and tropics).
    class GeographicGrid final : public Layer
    {
    public:
        GeographicGrid(SpatialReference*);

        std::vector<const Entity*> entities() const override;

    private:
        size_t _latitudeInterval;
        size_t _longitudeInterval;
        SpatialReference* _srs;

        std::vector<std::unique_ptr<Entity>> _parallels;

        void createEntities();
    };
}

#endif