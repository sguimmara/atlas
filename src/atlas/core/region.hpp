#pragma once
#ifndef ATLAS_CORE_REGION_HPP
#define ATLAS_CORE_REGION_HPP

#include "common.hpp"
#include "cartographic.hpp"

namespace atlas::core
{
    class Region
    {
    public:
        Region();
        Region(Cartographic min, Cartographic max);
        Region(Cartographic min, rad width, meters height);

        static Region combine(Region a, Region b);
        static Region world();

        void expand(Region& other);
        bool intersects(Region& other);
        bool contains(Cartographic point);

        // returns a region whose altitudes are raised by the specified amount.
        Region raise(meters height) const noexcept;

        // subdivide the region in a grid of x horizontal cells in width, and y vertical cells.
        std::vector<Region> subdivide(size_t x, size_t y) const noexcept;

        inline bool operator==(const Region& rhs) const noexcept { return rhs._min == _min && rhs._max == _max; }
        inline bool operator!=(const Region& rhs) const noexcept { return !(*this == rhs); }
        inline const Cartographic min() const noexcept { return _min; }
        inline const Cartographic max() const noexcept { return _max; }
        const Cartographic topLeft() const noexcept;
        const Cartographic topRight() const noexcept;
        const Cartographic bottomLeft() const noexcept;
        const Cartographic bottomRight() const noexcept;
        inline const double width() const noexcept { return std::abs(_max.longitude - _min.longitude); }
        inline const double height() const noexcept { return std::abs(_max.latitude - _min.latitude); }
        inline const double east() const noexcept { return _max.longitude; }
        inline const double west() const noexcept { return _min.longitude; }
        inline const double north() const noexcept { return _max.latitude; }
        inline const double south() const noexcept { return _min.latitude; }
        Region northWest() const noexcept;
        Region northEast() const noexcept;
        Region southWest() const noexcept;
        Region southEast() const noexcept;

        Cartographic center() const noexcept { return Cartographic::midpoint(_min, _max); }

    private:
        Cartographic _min;
        Cartographic _max;
    };
}

#endif