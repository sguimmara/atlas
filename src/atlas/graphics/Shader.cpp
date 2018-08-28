#include "Shader.hpp"
#include "atlas/core/IO.hpp"

namespace atlas
{
    namespace graphics
    {
        static vk::ShaderModule CreateShaderModule(const std::vector<char>& code, const vk::Device device)
        {
            auto const info = vk::ShaderModuleCreateInfo()
                .setCodeSize(code.size())
                .setPCode(reinterpret_cast<const uint32_t*>(code.data()));

            vk::ShaderModule module;
            auto result = device.createShaderModule(&info, nullptr, &module);
            VERIFY(result == vk::Result::eSuccess);
            return module;
        }

        Shader::Shader(const std::string name, const std::string path, const vk::Device device) :
            _name(name),
            _device(device)
        {
            auto spirv = core::IO::readAllBytes(path);
            _shaderModule = CreateShaderModule(spirv, _device);
            spdlog::get("renderer")->debug("loaded shader module '{0}'", path);

        }

        Shader::~Shader()
        {
            _device.destroyShaderModule(_shaderModule);
        }
    }
}
