#include <json/json.h>

#include "global_properties.hpp"
#include "instance_properties.hpp"
#include "pipeline.hpp"
#include "instance.hpp"
#include "atlas/io/utils.hpp"

using namespace atlas::renderer;
using namespace spirv_cross;

std::unordered_map<std::string, std::unique_ptr<Pipeline>> Pipeline::_cache;
std::shared_ptr<spdlog::logger> Pipeline::_log = nullptr;
vk::DescriptorSetLayout Pipeline::_globalPropertyLayout;
vk::Buffer Pipeline::_globalPropertyBuffer;
vk::DescriptorSet Pipeline::_globalPropertySet;
vk::DescriptorSetLayout Pipeline::_instancePropertyLayout;

struct BindingInfoDescription
{
    size_t size;
    uint32_t set;
};

struct InputInfo
{
    size_t size;
    size_t vecsize;
    spirv_cross::SPIRType::BaseType basetype;
    vk::Format format;
    uint32_t offset;
};

const std::unordered_set<std::string> acceptedSamplerNamesVS
{
};
const std::unordered_set<std::string> acceptedSamplerNamesFS
{
    "diffuse", "specular"
};
const std::unordered_map<std::string, BindingInfoDescription> recognizedUniformsGlobals
{
    { "globals", { sizeof(GlobalProperties), 0 } },
    { "instance", { sizeof(InstanceProperties), 1 } }
};
const std::unordered_map<std::string, BindingInfoDescription> recognizedUniformsVS
{
};
const std::unordered_map<std::string, BindingInfoDescription> recognizedUniformsFS
{
    { "material", { sizeof(float), 2 } }
};
const std::unordered_map<std::string, InputInfo> recognizedInputsVS
{
    { "in_position", { sizeof(glm::vec3), 3, SPIRType::BaseType::Float, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, position) } },
    { "in_normal",   { sizeof(glm::vec3), 3, SPIRType::BaseType::Float, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, normal) } },
    { "in_st",       { sizeof(glm::vec2), 2, SPIRType::BaseType::Float, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, st) } }
};

void Pipeline::initialize()
{
    _globalPropertyBuffer = Allocator::getBuffer(sizeof(GlobalProperties), vk::BufferUsageFlagBits::eUniformBuffer);

    auto binding = vk::DescriptorSetLayoutBinding()
        .setBinding(0)
        .setDescriptorCount(1)
        .setDescriptorType(vk::DescriptorType::eUniformBuffer)
        .setStageFlags(vk::ShaderStageFlagBits::eAllGraphics);

    auto layoutInfo = vk::DescriptorSetLayoutCreateInfo()
        .setBindingCount(1)
        .setPBindings(&binding);

    _globalPropertyLayout = Instance::device.createDescriptorSetLayout(layoutInfo);

    auto const allocateInfo = vk::DescriptorSetAllocateInfo()
        .setDescriptorPool(Instance::descriptorPool)
        .setDescriptorSetCount(1)
        .setPSetLayouts(&_globalPropertyLayout);

    _globalPropertySet = Instance::device.allocateDescriptorSets(allocateInfo)[0];

    auto instanceBinding = vk::DescriptorSetLayoutBinding()
        .setBinding(1)
        .setDescriptorCount(1)
        .setDescriptorType(vk::DescriptorType::eUniformBuffer)
        .setStageFlags(vk::ShaderStageFlagBits::eAllGraphics);

    auto instanceLayoutInfo = vk::DescriptorSetLayoutCreateInfo()
        .setBindingCount(1)
        .setPBindings(&instanceBinding);

    _instancePropertyLayout = Instance::device.createDescriptorSetLayout(instanceLayoutInfo);
}

Pipeline * Pipeline::get(const std::string & name)
{
    if (_cache.count(name) == 0)
    {
        getLog()->warn("the pipeline '{0}' does not exist. Was it created properly ?", name);
        return nullptr;
    }

    getLog()->trace("pipeline '{0}' retrieved successfully", name);
    return _cache[name].get();
}

Pipeline * Pipeline::create(const std::string & json)
{
    auto pipeline = std::make_unique<Pipeline>(json);
    auto ptr = pipeline.get();
    getLog()->debug("pipeline '{0}' created successfully", ptr->_name);
    _cache.insert({ pipeline->_name, std::move(pipeline) });
    return ptr;
}

spdlog::logger* Pipeline::getLog()
{
    if (_log == nullptr)
    {
        _log = spdlog::stdout_color_mt("pipeline");
    }
    return _log.get();
}

vk::ShaderModule createShaderModule(std::vector<char> code)
{
    return Instance::device.createShaderModule(vk::ShaderModuleCreateInfo()
        .setCodeSize(code.size())
        .setPCode(reinterpret_cast<const uint32_t*>(code.data())));
}

void Pipeline::terminate()
{
    Allocator::free(_globalPropertyBuffer);
    Instance::device.destroyDescriptorSetLayout(_globalPropertyLayout);
    Instance::device.destroyDescriptorSetLayout(_instancePropertyLayout);
    _cache.clear();
}

vk::PipelineColorBlendAttachmentState defaultBlendAttachment()
{
    return vk::PipelineColorBlendAttachmentState()
        .setBlendEnable(VK_FALSE)
        .setColorWriteMask(
            vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
            vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
}

vk::PipelineRasterizationStateCreateInfo defaultRasterizer()
{
    return vk::PipelineRasterizationStateCreateInfo()
        .setCullMode(vk::CullModeFlagBits::eBack)
        .setFrontFace(vk::FrontFace::eClockwise)
        .setLineWidth(1)
        .setPolygonMode(vk::PolygonMode::eFill);
}

vk::CullModeFlags parseCullMode(Json::Value json)
{
    if (json.empty()) return vk::CullModeFlagBits::eBack;
    if (json.asString() == "front") return vk::CullModeFlagBits::eFront;
    if (json.asString() == "back") return vk::CullModeFlagBits::eBack;
    if (json.asString() == "none") return vk::CullModeFlagBits::eNone;
    throw std::runtime_error("unrecognized CullMode value: " + json.asString());
}

vk::FrontFace parseFrontFace(Json::Value json)
{
    if (json.empty()) return vk::FrontFace::eClockwise;
    if (json.asString() == "cw") return vk::FrontFace::eClockwise;
    if (json.asString() == "ccw") return vk::FrontFace::eCounterClockwise;
    throw std::runtime_error("unrecognized FrontFace value: " + json.asString());
}

vk::PolygonMode parsePolygonMode(Json::Value json)
{
    if (json.empty()) return vk::PolygonMode::eFill;
    if (json.asString() == "fill") return vk::PolygonMode::eFill;
    if (json.asString() == "line") return vk::PolygonMode::eLine;
    if (json.asString() == "point") return vk::PolygonMode::ePoint;
    throw std::runtime_error("unrecognized PolygonMode value: " + json.asString());
}

float parse(Json::Value json, float defaultValue)
{
    if (json.empty()) return defaultValue;
    return json.asFloat();
}

vk::PipelineRasterizationStateCreateInfo parseRasterizer(Json::Value json)
{
    if (json.empty())
    {
        return defaultRasterizer();
    }

    return vk::PipelineRasterizationStateCreateInfo()
        .setCullMode(parseCullMode(json["cullMode"]))
        .setFrontFace(parseFrontFace(json["frontFace"]))
        .setLineWidth(parse(json["lineWidth"], 1.0f))
        .setPolygonMode(parsePolygonMode(json["polygonMode"]));
}

vk::PipelineInputAssemblyStateCreateInfo defaultAssemblyState()
{
    return vk::PipelineInputAssemblyStateCreateInfo()
        .setTopology(vk::PrimitiveTopology::eTriangleList)
        .setPrimitiveRestartEnable(VK_FALSE);
}

vk::PipelineDepthStencilStateCreateInfo defaultStencilState()
{
    return vk::PipelineDepthStencilStateCreateInfo()
        .setDepthTestEnable(VK_TRUE)
        .setDepthWriteEnable(VK_TRUE)
        .setDepthCompareOp(vk::CompareOp::eLess)
        .setStencilTestEnable(VK_FALSE);
}

vk::PipelineMultisampleStateCreateInfo defaultMultisampleState()
{
    return vk::PipelineMultisampleStateCreateInfo()
        .setSampleShadingEnable(VK_FALSE)
        .setRasterizationSamples(vk::SampleCountFlagBits::e1);
}

vk::PipelineViewportStateCreateInfo defaultViewportState(vk::Viewport viewport)
{
    auto const scissor = vk::Rect2D(
        vk::Offset2D(0, 0),
        vk::Extent2D(static_cast<uint32_t>(viewport.width), static_cast<uint32_t>(viewport.height)));

    return vk::PipelineViewportStateCreateInfo()
        .setPScissors(&scissor)
        .setScissorCount(1)
        .setViewportCount(1)
        .setPViewports(&viewport);
}

std::vector<uint32_t> readSpirv(std::vector<char> rawData)
{
    std::vector<uint32_t> result(rawData.size() / 4);
    memcpy(result.data(), rawData.data(), rawData.size());
    return result;
}

std::vector<vk::VertexInputAttributeDescription> getVertexInputAttributes(
    const spirv_cross::Compiler& comp,
    const spirv_cross::ShaderResources& resources)
{
    auto attributes = std::vector<vk::VertexInputAttributeDescription>();

    for (const auto& input : resources.stage_inputs)
    {
        const auto name = input.name;
        if (recognizedInputsVS.count(name) == 0)
        {
            throw std::runtime_error("unrecognized vertex input: " + name);
        }

        const auto bi = recognizedInputsVS.find(name)->second;
        const auto loca = comp.get_decoration(input.id, spv::Decoration::DecorationLocation);
        const auto type = comp.get_type(input.type_id);
        assert(type.vecsize == bi.vecsize);
        assert(type.basetype == bi.basetype);

        const auto description = vk::VertexInputAttributeDescription()
            .setBinding(0)
            .setLocation(loca)
            .setOffset(bi.offset)
            .setFormat(bi.format);

        attributes.push_back(description);
    }

    return attributes;
}

std::vector<vk::PushConstantRange> getPushConstantRanges(
    const spirv_cross::Compiler& comp,
    const spirv_cross::ShaderResources& resources,
    const vk::ShaderStageFlagBits& stage)
{
    auto result = std::vector<vk::PushConstantRange>();

    for (const auto& buf : resources.push_constant_buffers)
    {
        //auto name = comp.get_name(buf.id);
        //if (name == "instance")
        //{
        //    if (stage != vk::ShaderStageFlagBits::eVertex)
        //    {
        //        throw std::runtime_error("unexpected push constant block. should be in the vertex stage.");
        //    }

        //    auto type = comp.get_type(buf.type_id);
        //    auto size = comp.get_declared_struct_size(type);

        //    assert(size == sizeof(InstanceProperties));

        //    auto const range = vk::PushConstantRange()
        //        .setOffset(0)
        //        .setSize(sizeof(PerInstanceBlock))
        //        .setStageFlags(vk::ShaderStageFlagBits::eVertex);

        //    result.emplace_back(range);
        //}
        //else
        //{
        //    throw std::runtime_error("unknown push constant range: " + name);
        //}
    }

    return result;
}

vk::DescriptorSetLayoutBinding Pipeline::getSampler(
    const spirv_cross::Compiler& comp,
    const spirv_cross::Resource& sampler,
    const vk::ShaderStageFlagBits& stage)
{
    auto name = comp.get_name(sampler.id);
    auto bind = comp.get_decoration(sampler.id, spv::Decoration::DecorationBinding);
    auto set = comp.get_decoration(sampler.id, spv::Decoration::DecorationDescriptorSet);

    if (stage == vk::ShaderStageFlagBits::eFragment && acceptedSamplerNamesFS.count(name) == 0)
    {
        throw std::runtime_error("unrecognized fragment stage sampler name: " + name);
    }
    else  if (stage == vk::ShaderStageFlagBits::eVertex && acceptedSamplerNamesVS.count(name) == 0)
    {
        throw std::runtime_error("unrecognized vertex stage sampler name: " + name);
    }

    add(BindingInfo(name, bind, set, vk::DescriptorType::eCombinedImageSampler));

    auto bindingInfo = vk::DescriptorSetLayoutBinding()
        .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
        .setBinding(bind)
        .setDescriptorCount(1)
        .setStageFlags(stage);

    return bindingInfo;
}

vk::DescriptorSetLayoutBinding Pipeline::getUniform(
    const spirv_cross::Compiler& comp,
    const spirv_cross::Resource& res,
    const vk::ShaderStageFlagBits& stage)
{
    auto name = comp.get_name(res.id);
    auto bind = comp.get_decoration(res.id, spv::Decoration::DecorationBinding);
    auto set = comp.get_decoration(res.id, spv::Decoration::DecorationDescriptorSet);
    auto type = comp.get_type(res.type_id);
    auto size = comp.get_declared_struct_size(type);

    BindingInfoDescription bi;
    vk::ShaderStageFlags shaderStages = stage;
    if (recognizedUniformsGlobals.count(name) == 1)
    {
        shaderStages = vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex;
        bi = recognizedUniformsGlobals.find(name)->second;
    }
    else if (stage == vk::ShaderStageFlagBits::eFragment)
    {
        if (recognizedUniformsFS.count(name) == 0)
        {
            throw std::runtime_error("unrecognized fragment stage uniform name: " + name);
        }
        bi = recognizedUniformsFS.find(name)->second;
    }
    else if (stage == vk::ShaderStageFlagBits::eVertex)
    {
        if (recognizedUniformsVS.count(name) == 0)
        {
            throw std::runtime_error("unrecognized vertex stage uniform name: " + name);
        }
        bi = recognizedUniformsVS.find(name)->second;
    }

    assert(bi.size == size);
    assert(bi.set == set);

    auto pair = std::pair<uint32_t, uint32_t>(set, bind);

    auto const ub = BindingInfo(name, bind, set, vk::DescriptorType::eUniformBuffer);

    if (_bindings.count(ub) == 0)
    {
        add(ub);
    }
    else
    {
        return VK_NULL_HANDLE;
    }

    auto bindingInfo = vk::DescriptorSetLayoutBinding()
        .setDescriptorType(vk::DescriptorType::eUniformBuffer)
        .setBinding(bind)
        .setDescriptorCount(1)
        .setStageFlags(shaderStages);

    return bindingInfo;
}

std::vector<vk::DescriptorSetLayoutBinding> Pipeline::getLayout(
    const spirv_cross::Compiler& comp,
    const spirv_cross::ShaderResources& resources,
    const vk::ShaderStageFlagBits& stage)
{
    auto bindings = std::vector<vk::DescriptorSetLayoutBinding>();

    for (const auto& s : resources.sampled_images)
    {
        bindings.push_back(getSampler(comp, s, stage));
    }
    for (const auto& u : resources.uniform_buffers)
    {
        auto uniform = getUniform(comp, u, stage);
        if (uniform != VK_NULL_HANDLE)
        {
            bindings.push_back(uniform);
        }
    }

    return bindings;
}

template<typename T>
std::vector<T> concat(const std::vector<T>& lhs, const std::vector<T>& rhs)
{
    std::vector<T> result(lhs);
    result.insert(result.end(), rhs.begin(), rhs.end());
    return result;
}

void Pipeline::add(BindingInfo u) noexcept
{
    _bindings.insert(u);
}

uint32_t Pipeline::getSamplerBinding(const std::string & name) const
{
    for (const auto& ub : _bindings)
    {
        if (ub.name == name)
        {
            if (ub.type != vk::DescriptorType::eCombinedImageSampler)
            {
                throw std::runtime_error("binding <" + name + "> is not of type sampler");
            }

            return ub.index;
        }
    }

    throw std::runtime_error("no sampler binding found with name <" + name + ">");
}

uint32_t Pipeline::getBindingInfo(const std::string & name) const
{
    for (const auto& ub : _bindings)
    {
        if (ub.name == name)
        {
            if (ub.type != vk::DescriptorType::eUniformBuffer)
            {
                throw std::runtime_error("binding <" + name + "> is not of type uniform");
            }

            return ub.index;
        }
    }

    throw std::runtime_error("no uniform binding found with name <" + name + ">");
}


Pipeline::Pipeline(const std::string& json) :
    _json(json)
{
    Json::CharReaderBuilder builder;
    std::unique_ptr<Json::CharReader> reader = std::unique_ptr<Json::CharReader>(builder.newCharReader());
    Json::Value root;
    std::string errs;

    if (reader->parse(json.c_str(), json.c_str() + json.size(), &root, &errs))
    {
        _name = root["name"].asString();
        auto const vertexShaderPath = Instance::shaderDirectory() + root["vertex"].asString();
        auto const fragmentShaderPath = Instance::shaderDirectory() + root["fragment"].asString();

        auto const vsSpirv = atlas::io::Utils::read(vertexShaderPath);
        auto const fsSpirv = atlas::io::Utils::read(fragmentShaderPath);

        _vertexShader = createShaderModule(vsSpirv);
        _fragmentShader = createShaderModule(fsSpirv);

        spirv_cross::Compiler vs(readSpirv(vsSpirv));
        auto vsResources = vs.get_shader_resources();

        spirv_cross::Compiler fs(readSpirv(fsSpirv));
        auto fsResources = fs.get_shader_resources();

        // push constant ranges
        auto vsConstantRanges = getPushConstantRanges(vs, vsResources, vk::ShaderStageFlagBits::eVertex);
        auto fsConstantRanges = getPushConstantRanges(fs, fsResources, vk::ShaderStageFlagBits::eFragment);
        auto constantRanges = concat(vsConstantRanges, fsConstantRanges);

        // descriptor sets
        auto vsBindings = getLayout(vs, vsResources, vk::ShaderStageFlagBits::eVertex);
        auto fsBindings = getLayout(fs, fsResources, vk::ShaderStageFlagBits::eFragment);
        auto bindings = concat(vsBindings, fsBindings);

        _attributes = getVertexInputAttributes(vs, vsResources);

        // layouts
        auto const layoutInfo = vk::DescriptorSetLayoutCreateInfo()
            .setBindingCount(uint32_t(bindings.size()))
            .setPBindings(bindings.data());

        _descriptorSetLayout = Instance::device.createDescriptorSetLayout(layoutInfo);

        std::array<vk::DescriptorSetLayout, 3> setLayouts = { _globalPropertyLayout, _instancePropertyLayout, _descriptorSetLayout };

        auto pipelineLayoutInfo = vk::PipelineLayoutCreateInfo()
            .setPPushConstantRanges(constantRanges.data())
            .setPushConstantRangeCount(uint32_t(constantRanges.size()))
            .setSetLayoutCount((uint32_t)setLayouts.size())
            .setPSetLayouts(setLayouts.data());

        _layout = Instance::device.createPipelineLayout(pipelineLayoutInfo);

        const auto assemblyState = defaultAssemblyState();
        const auto rasterizer = parseRasterizer(root["rasterizer"]);
        const auto stencil = defaultStencilState();
        const auto msaa = defaultMultisampleState();
        const auto viewportState = defaultViewportState(Instance::context()->viewport());

        // shader stages
        auto const vsInfo = vk::PipelineShaderStageCreateInfo()
            .setModule(_vertexShader)
            .setStage(vk::ShaderStageFlagBits::eVertex)
            .setPName("main");

        auto const fsInfo = vk::PipelineShaderStageCreateInfo()
            .setModule(_fragmentShader)
            .setStage(vk::ShaderStageFlagBits::eFragment)
            .setPName("main");

        std::array<vk::PipelineShaderStageCreateInfo, 2> stages =
        {
            vsInfo, fsInfo
        };

        std::array<vk::DynamicState, 2> dynamicStates = {
            vk::DynamicState::eViewport,
            vk::DynamicState::eScissor };

        const auto dynamicState = vk::PipelineDynamicStateCreateInfo()
            .setDynamicStateCount(uint32_t(dynamicStates.size()))
            .setPDynamicStates(dynamicStates.data());

        const auto blendAttachment = defaultBlendAttachment();
        const auto blendState = vk::PipelineColorBlendStateCreateInfo()
            .setAttachmentCount(1)
            .setPAttachments(&blendAttachment);

        const auto binding = vk::VertexInputBindingDescription()
            .setBinding(0)
            .setInputRate(vk::VertexInputRate::eVertex)
            .setStride(sizeof(Vertex));

        const auto vertexInputState = vk::PipelineVertexInputStateCreateInfo()
            .setVertexAttributeDescriptionCount(uint32_t(_attributes.size()))
            .setPVertexAttributeDescriptions(_attributes.data())
            .setVertexBindingDescriptionCount(1)
            .setPVertexBindingDescriptions(&binding);

        auto const pipelineInfo = vk::GraphicsPipelineCreateInfo()
            .setLayout(_layout)
            .setPStages(stages.data())
            .setStageCount(uint32_t(stages.size()))
            .setPRasterizationState(&rasterizer)
            .setPColorBlendState(&blendState)
            .setPDepthStencilState(&stencil)
            .setPMultisampleState(&msaa)
            .setPViewportState(&viewportState)
            .setPVertexInputState(&vertexInputState)
            .setPInputAssemblyState(&assemblyState)
            .setPDynamicState(&dynamicState)
            .setRenderPass(Instance::renderPass);

        _pipeline = Instance::device.createGraphicsPipeline(nullptr, pipelineInfo);
        getLog()->debug("pipeline successfully created");
    }
    else
    {
        getLog()->error("an error occurred while parsing material description: {0}", errs);
    }
}

Pipeline::~Pipeline()
{
    Instance::device.destroyShaderModule(_fragmentShader);
    Instance::device.destroyShaderModule(_vertexShader);
    Instance::device.destroyPipelineLayout(_layout);
    Instance::device.destroyPipeline(_pipeline);
    Instance::device.destroyDescriptorSetLayout(_descriptorSetLayout);
}
