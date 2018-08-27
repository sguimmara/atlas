#ifndef ATLAS_SHADER_HPP
#define ATLAS_SHADER_HPP

#include "AtlasGraphics.hpp"

namespace atlas
{
namespace graphics
{
    /**
    * @brief       Encapsulates a Vulkan shader module.
    */
    class Shader
    {
    public:
        /**
        * @brief       Loads a shader from a SPIR-V file
        */
        Shader(const std::string name, const std::string path, const vk::Device device);
        ~Shader();

        /**
        * @brief       Returns the underlying Vulkan shader module
        */
        inline vk::ShaderModule shaderModule() const noexcept { return _shaderModule; }

    private:
        std::string _name;
        vk::ShaderModule _shaderModule;
        vk::Device _device;
    };
}
}
#endif
