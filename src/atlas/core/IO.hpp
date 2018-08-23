#ifndef ATLAS_IO_HPP
#define ATLAS_IO_HPP

#include <vector>
#include <string>

namespace atlas
{
    namespace core
    {
        /**
        * @brief      Utilities for reading and writing files.
        */
        class IO
        {
        public:
            static std::vector<char> readAllBytes(const std::string& filename);
        };
    }
}
#endif