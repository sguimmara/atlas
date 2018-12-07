#ifndef ATLAS_IO_RESPONSE_HPP
#define ATLAS_IO_RESPONSE_HPP

#include "common.hpp"

namespace atlas::io
{
    template<typename T>
    class Response
    {
    public:
        Response(T data, void* userData) : _data(data), _userData(userData) {}

        inline void* userData() const noexcept { return _userData; }
        inline T data() const noexcept { return _data; }

    private:
        void* _userData;
        T _data;
    };
}

#endif