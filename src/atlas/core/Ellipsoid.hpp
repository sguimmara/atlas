#ifndef ATLAS_ELLIPSOID_HPP
#define ATLAS_ELLIPSOID_HPP

#include <vector>
#include <string>

#include "Constants.hpp"

namespace atlas
{
    namespace core
    {
        /**
        * @brief      Defines an ellipsoid.
        */
        class Ellipsoid
        {
        public:
            Ellipsoid(double semimajorAxis, double semiminorAxis);

            inline double semimajorAxis() const { return _semimajorAxis; }
            inline double semiminorAxis() const { return _semiminorAxis; }

            static Ellipsoid GRS80;

        private:
            double _semimajorAxis;
            double _semiminorAxis;
        };
    }
}
#endif