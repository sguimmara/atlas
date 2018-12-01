#ifndef ATLAS_IO_UTILS_HPP
#define ATLAS_IO_UTILS_HPP

#include <string>
#include <fstream>
#include <vector>

namespace atlas::io
{
    // utility functions to handle files
    class Utils
    {
    public:
        static std::vector<char> read(const std::string& path);
    };
}

#endif // ATLAS_IO_UTILS_HPP