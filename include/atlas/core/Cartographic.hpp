#ifndef ATLAS_GEOGRAPHIC_HPP
#define ATLAS_GEOGRAPHIC_HPP

namespace atlas
{

    /**
     * @brief      Represents a coordinate in an ellipsoidal reference.
     */
    class Cartographic
    {
        public:

            /**
             * @brief      constructs a default coordinate
             */
            Cartographic() noexcept :
                mLon(0),
                mLat(0),
                mAlt(0)
            {

            }

            /**
             * @brief      constructs the coordinate
             *
             * @param[in]  lon   The longitude, in degrees
             * @param[in]  lat   The latitude, in degrees
             * @param[in]  alt   The altitude, in meters
             */
            Cartographic(double lon, double lat, double alt = 0) noexcept :
                mLon(lon),
                mLat(lat),
                mAlt(alt)
            {
            }

            /**
             * @brief      Returns the longitude, in degrees
             *
             * @return     the longitude in degrees
             */
            inline double longitude() const noexcept
            {
                return mLon;
            }

            /**
             * @brief      Returns the latitude, in degrees
             *
             * @return     the latitude in degrees
             */
            inline double latitude() const noexcept
            {
                return mLat;
            }

            /**
             * @brief      Returns the altitude, in meters
             *
             * @return     the altitude in meters
             */
            inline double altitude() const noexcept
            {
                return mAlt;
            }

        private:
            double mLon;
            double mLat;
            double mAlt;
    };

}

#endif // !ATLAS_GEOGRAPHIC_HPP