#include "Material.hpp"
#include "Renderer.hpp"
#include "Vertex.hpp"

using namespace atlas::graphics;

vk::PipelineViewportStateCreateInfo CreateViewportState(vk::Viewport viewport)
{
    auto const scissor = vk::Rect2D(
        vk::Offset2D(0, 0),
        vk::Extent2D(static_cast<uint32_t>(viewport.width), static_cast<uint32_t>(viewport.height)));
    auto const viewportState = vk::PipelineViewportStateCreateInfo()
        .setPScissors(&scissor)
        .setScissorCount(1)
        .setViewportCount(1)
        .setPViewports(&viewport);

    return viewportState;
}

vk::PipelineColorBlendStateCreateInfo CreateColorBlendState()
{
    auto const blendAttachment = vk::PipelineColorBlendAttachmentState()
        .setBlendEnable(VK_FALSE)
        .setColorWriteMask(
            vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
            vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);

    auto const blending = vk::PipelineColorBlendStateCreateInfo()
        .setAttachmentCount(1)
        .setPAttachments(&blendAttachment);

    return blending;
}

vk::PipelineRasterizationStateCreateInfo CreateRasterizationState(vk::PolygonMode polygonMode, float lineWidth)
{
    auto const rasterization = vk::PipelineRasterizationStateCreateInfo()
        .setPolygonMode(polygonMode)
        .setLineWidth(lineWidth)
        .setCullMode(vk::CullModeFlagBits::eBack)
        .setFrontFace(vk::FrontFace::eClockwise);

    return rasterization;
}

vk::PipelineDepthStencilStateCreateInfo CreateDepthStencilState()
{
    auto const depthStencil = vk::PipelineDepthStencilStateCreateInfo()
        .setDepthTestEnable(VK_TRUE)
        .setDepthWriteEnable(VK_TRUE)
        .setDepthCompareOp(vk::CompareOp::eLess)
        .setStencilTestEnable(VK_FALSE);

    return depthStencil;
}

vk::PipelineDynamicStateCreateInfo CreateDynamicStates()
{
    std::array<vk::DynamicState, 2> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eLineWidth };
    auto const dynamicState = vk::PipelineDynamicStateCreateInfo()
        .setDynamicStateCount(static_cast<uint32_t>(dynamicStates.size()))
        .setPDynamicStates(dynamicStates.data());

    return dynamicState;
}

void Material::CreateDescriptorSet(size_t swapchainSize, std::vector<Descriptor> descriptors, vk::DescriptorPool pool)
{
    std::vector<vk::DescriptorSetLayoutBinding> bindings;

    for (uint32_t i = 0; i < descriptors.size(); i++)
    {
        auto const descriptor = vk::DescriptorSetLayoutBinding()
            .setBinding(i)
            .setDescriptorCount(1)
            .setDescriptorType(descriptors[i].type)
            .setStageFlags(descriptors[i].flags)
            .setPImmutableSamplers(nullptr);

        bindings.push_back(descriptor);
    }

    auto const layoutInfo = vk::DescriptorSetLayoutCreateInfo()
        .setBindingCount(static_cast<uint32_t>(bindings.size()))
        .setPBindings(bindings.data());

    CHECK_SUCCESS(Renderer::device.createDescriptorSetLayout(&layoutInfo, nullptr, &_layout));

    auto const allocInfo = vk::DescriptorSetAllocateInfo()
        .setDescriptorPool(Renderer::current->descriptorPool())
        .setDescriptorSetCount(static_cast<uint32_t>(swapchainSize))
        .setPSetLayouts(&_layout);

    Renderer::device.allocateDescriptorSets(&allocInfo, _descriptorSets.data());
}

Material::Material()
{
}

Material::Material(std::vector<Semantic> semantics, std::vector<Descriptor> bindings, Shader vs, Shader fs, vk::PrimitiveTopology topology) :
    lineWidth(1.0f)
{
    std::vector<vk::VertexInputAttributeDescription> attributes;
    auto const binding = vk::VertexInputBindingDescription()
        .setBinding(0)
        .setStride(sizeof(Vertex))
        .setInputRate(vk::VertexInputRate::eVertex);

    for (uint32_t i = 0; i < semantics.size(); i++)
    {
        uint32_t offset;
        vk::Format format;

        switch (semantics[i])
        {
        case Semantic::Position:
            offset = offsetof(Vertex, position);
            format = vk::Format::eR32G32B32Sfloat;
            break;
        case Semantic::Normal:
        case Semantic::Color:
            offset = offsetof(Vertex, normal);
            format = vk::Format::eR32G32B32Sfloat;
            break;
        case Semantic::TexCoord:
            offset = offsetof(Vertex, uv);
            format = vk::Format::eR16G16Sfloat;
        default:
            break;
        }

        auto const attribute = vk::VertexInputAttributeDescription()
            .setBinding(0)
            .setLocation(i)
            .setFormat(format)
            .setOffset(offset);

        attributes.push_back(attribute);
    }

    auto const vertexInputs = vk::PipelineVertexInputStateCreateInfo()
        .setVertexAttributeDescriptionCount(static_cast<uint32_t>(attributes.size()))
        .setPVertexAttributeDescriptions(attributes.data())
        .setVertexBindingDescriptionCount(1)
        .setPVertexBindingDescriptions(&binding);

    auto const assembly = vk::PipelineInputAssemblyStateCreateInfo()
        .setTopology(topology)
        .setPrimitiveRestartEnable(VK_FALSE);

    auto const vertexStage = vk::PipelineShaderStageCreateInfo()
        .setStage(vk::ShaderStageFlagBits::eVertex)
        .setModule(vs.module)
        .setPName("main");

    auto const fragmentStage = vk::PipelineShaderStageCreateInfo()
        .setStage(vk::ShaderStageFlagBits::eFragment)
        .setModule(fs.module)
        .setPName("main");

    vk::PipelineShaderStageCreateInfo stages[] = { vertexStage, fragmentStage };

    auto const viewportState = CreateViewportState(Renderer::viewport);

    auto const rasterization = CreateRasterizationState(vk::PolygonMode::eFill, lineWidth);

    auto const multisampling = vk::PipelineMultisampleStateCreateInfo()
        .setRasterizationSamples(vk::SampleCountFlagBits::e1);

    auto const blendAttachment = vk::PipelineColorBlendAttachmentState()
        .setBlendEnable(VK_FALSE)
        .setColorWriteMask(
            vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
            vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);

    auto const blending = vk::PipelineColorBlendStateCreateInfo()
        .setAttachmentCount(1)
        .setPAttachments(&blendAttachment);

    auto const stencil = CreateDepthStencilState();

    std::array<vk::DynamicState, 2> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eLineWidth };
    auto const dynamicState = vk::PipelineDynamicStateCreateInfo()
        .setDynamicStateCount(static_cast<uint32_t>(dynamicStates.size()))
        .setPDynamicStates(dynamicStates.data());

    //CreateDescriptorSet(Renderer::current->swapchainSize(), bindings, Renderer::current->descriptorPool());

    auto const constantRange = vk::PushConstantRange()
        .setOffset(0)
        .setStageFlags(vk::ShaderStageFlagBits::eVertex)
        .setSize(sizeof(MVP));

    auto const pipelineLayoutInfo = vk::PipelineLayoutCreateInfo()
        .setSetLayoutCount(0)
        //.setPSetLayouts(&_layout)
        .setPushConstantRangeCount(1)
        .setPPushConstantRanges(&constantRange);

    CHECK_SUCCESS(Renderer::device.createPipelineLayout(&pipelineLayoutInfo, nullptr, &pipelineLayout));

    auto const pipelineInfo = vk::GraphicsPipelineCreateInfo()
        .setStageCount(2)
        .setPStages(stages)
        .setPVertexInputState(&vertexInputs)
        .setPInputAssemblyState(&assembly)
        .setPViewportState(&viewportState)
        .setPRasterizationState(&rasterization)
        .setPMultisampleState(&multisampling)
        .setPColorBlendState(&blending)
        .setLayout(pipelineLayout)
        .setPDynamicState(&dynamicState)
        .setRenderPass(Renderer::current->renderPass())
        .setPDepthStencilState(&stencil)
        .setSubpass(0);

    CHECK_SUCCESS(Renderer::device.createGraphicsPipelines(nullptr, 1, &pipelineInfo, nullptr, &pipeline));
}
