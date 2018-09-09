#ifndef ATLAS_SHADER_HPP
#define ATLAS_SHADER_HPP

#include "AtlasGraphics.hpp"
#include <unordered_map>

namespace atlas
{
namespace graphics
{
    /**
    * @brief       Encapsulates a Vulkan shader module.
    */
    struct Shader
    {
    public:
        std::string name;
        vk::ShaderModule module;

        static void SetDirectory(std::string directory);
        static Shader Get(std::string name, vk::Device device);

    private:
        static std::unordered_map<std::string, Shader> _store;
        static std::string _directory;
    };
}
}
#endif
