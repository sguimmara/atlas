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
        Shader(const std::string name, const std::string path, const VkDevice device);
        ~Shader();

        /**
        * @brief       Returns the underlying Vulkan shader module
        */
        inline VkShaderModule shaderModule() const noexcept { return this->_shaderModule; }

    private:
        std::string name;
        VkShaderModule _shaderModule;
        VkDevice device;
    };
}
}
#endif
