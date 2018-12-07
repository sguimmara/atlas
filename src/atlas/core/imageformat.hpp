#ifndef ATLAS_CORE_IMAGEFORMAT_HPP
#define ATLAS_CORE_IMAGEFORMAT_HPP

#include "common.hpp"

namespace atlas::core
{
    enum ImageFormat
    {
        Unknown,
        RGB24,
        RGBA32
    };

    static size_t getBpp(ImageFormat format)
    {
        switch (format)
        {
        case atlas::core::RGB24: return 3;
        case atlas::core::RGBA32: return 4;
        default: throw std::runtime_error("unimplemented image format");
        }
    }
}

#endif