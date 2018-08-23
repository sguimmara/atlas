#include "Shader.hpp"
#include "atlas/core/IO.hpp"

namespace atlas
{
namespace graphics
{
    static VkShaderModule createShaderModule(const std::vector<char>& code, const VkDevice device)
    {
        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VkShaderModule shaderModule;
        VK_CHECK_RESULT(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule));
        return shaderModule;
    }

    Shader::Shader(const std::string name, const std::string path, const VkDevice device) :
        name(name),
        device(device)
    {
        auto spirv = core::IO::readAllBytes(path);
        _shaderModule = createShaderModule(spirv, device);
    }

    Shader::~Shader()
    {
        vkDestroyShaderModule(this->device, this->_shaderModule, nullptr);
    }
}
}
