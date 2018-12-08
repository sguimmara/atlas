#ifndef ATLAS_RENDERER_PIPELINE_HPP
#define ATLAS_RENDERER_PIPELINE_HPP

#include "common.hpp"
#include "vertex.hpp"
#include "bindinginfo.hpp"
#include <unordered_set>
#include <spirv-cross/spirv_cross.hpp>

namespace atlas::renderer
{
    // encapsulates the pipeline state.
    class Pipeline
    {
    public:
        static void initialize();

        // returns the Pipeline with the given name, if any.
        static Pipeline* get(const std::string& name);

        // creates a pipeline from a JSON description, stores it in the cache,
        // then returns the created pipeline. If a pipeline with the same name
        // is already present in the cache, returns the cached pipeline instead.
        static Pipeline* create(const std::string& json);

        // destroys the pipeline cache and all associated pipelines.
        static void terminate();

        Pipeline(const std::string& json);
        ~Pipeline();

        uint32_t getSamplerBinding(const std::string& name) const;
        uint32_t getBindingInfo(const std::string& name) const;

        static vk::DescriptorSetLayout globalPropertyLayout() noexcept;
        static vk::DescriptorSetLayout entityPropertyLayout() noexcept;

        inline vk::DescriptorSetLayout descriptorSetLayout() const noexcept { return _descriptorSetLayout; }
        inline vk::Pipeline vkPipeline() const noexcept { return _pipeline; }
        inline vk::PipelineLayout layout() const noexcept { return _layout; }

    private:
        static std::unordered_map<std::string, std::shared_ptr<Pipeline>> _cache;
        static std::shared_ptr<spdlog::logger> _log;
        static spdlog::logger* getLog();
        static vk::DescriptorSetLayout _globalPropertyLayout;
        static vk::DescriptorSetLayout _entityPropertyLayout;

        std::string _json;
        std::string _name;
        vk::Pipeline _pipeline;
        vk::PipelineLayout _layout;
        vk::ShaderModule _fragmentShader;
        vk::ShaderModule _vertexShader;
        vk::DescriptorSetLayout _descriptorSetLayout;
        std::vector<vk::VertexInputAttributeDescription> _attributes;
        vk::DescriptorSetLayoutBinding getSampler(const spirv_cross::Compiler&, const spirv_cross::Resource&, const vk::ShaderStageFlagBits&);
        vk::DescriptorSetLayoutBinding getUniform(const spirv_cross::Compiler&, const spirv_cross::Resource&, const vk::ShaderStageFlagBits&);
        std::vector<vk::DescriptorSetLayoutBinding> getLayout(const spirv_cross::Compiler&, const spirv_cross::ShaderResources&, const vk::ShaderStageFlagBits&);
        void add(BindingInfo u) noexcept;

        std::unordered_set<BindingInfo> _bindings;
    };
}

#endif