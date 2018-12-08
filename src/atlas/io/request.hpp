#ifndef ATLAS_IO_IMAGEREQUEST_HPP
#define ATLAS_IO_IMAGEREQUEST_HPP

#include "atlas/core/region.hpp"

namespace atlas::io
{
    template<typename T>
    class Request
    {
    public:
        Request(T key, void* userData) : _key(key), _userData(userData) {}

        inline T key() const noexcept { return _key; }
        inline void* userData() const noexcept { return _userData; }

    private:
        T _key;
        void* _userData;
    };
}

#endif // ATLAS_IO_IMAGEREQUEST_HPP