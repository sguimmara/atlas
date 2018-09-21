#include "Shader.hpp"
#include "Renderer.hpp"
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

        std::string Shader::_directory = std::string("");
        std::unordered_map<std::string, Shader> Shader::_store = std::unordered_map<std::string, Shader>();

        void Shader::SetDirectory(std::string directory)
        {
            _directory = directory;
        }

        Shader Shader::Get(std::string name)
        {
            auto search = _store.find(name);
            if (search != _store.end())
            {
                return (*search).second;
            }
            else
            {
                std::string fullpath = _directory + name + ".spv";

                auto spirv = core::IO::readAllBytes(fullpath);
                auto module = CreateShaderModule(spirv, Renderer::device);
                spdlog::get("renderer")->debug("loaded shader module '{0}'", name);

                Shader shader = { name, module };
                _store.insert({ name, shader });
                return shader;
            }
        }

        void Shader::ClearStore()
        {
            for (auto shader : _store)
            {
                Renderer::device.destroyShaderModule(shader.second.module);
                spdlog::get("renderer")->debug("unloaded shader module '{0}'", shader.first);
            }
            _store.clear();
        }
    }
}
