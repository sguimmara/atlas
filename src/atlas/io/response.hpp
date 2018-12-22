#pragma once
#ifndef ATLAS_IO_RESPONSE_HPP
#define ATLAS_IO_RESPONSE_HPP

#include "common.hpp"

namespace atlas::io
{
    template<typename T>
    class Response
    {
    public:
        Response() : _data(nullptr), _userData(nullptr) {}
        Response(std::shared_ptr<T> data, void* userData) : _data(data), _userData(userData) {}

        inline std::shared_ptr<T> data() const noexcept { return _data; }
        inline void* userData() const noexcept { return _userData; }

    private:
        std::shared_ptr<T> _data;
        void* _userData;
    };
}

#endif