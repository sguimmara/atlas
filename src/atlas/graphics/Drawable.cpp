#include "Drawable.hpp"

#define SHADER_DIR "C:/Users/sguimmara/Documents/work/c++/atlas/build/msvc/bin/Debug/shaders/"

namespace atlas
{
    namespace graphics
    {
        Drawable::Drawable(Renderer* renderer) :
            _renderer(renderer),
            _fragmentShader(Shader("unlit-frag", SHADER_DIR "unlit.frag.spv", renderer->device())),
            _vertexShader(Shader("unlit-vert", SHADER_DIR "unlit.vert.spv", renderer->device())),
            GraphicsObject()
        {
            CreatePipeline(_vertexShader.shaderModule(), _fragmentShader.shaderModule());
            
            vk::PhysicalDevice gpu = _renderer->gpu();
            vk::Device device = _renderer->device();
            _mesh.indexCount = 6;
            _mesh.SetIndices(gpu, device, { 0, 2, 3, 0, 1, 2 });
            _mesh.SetPositions(gpu, device,
                {
                    { 0.1, 0.1, 0 },
                    { 0.1, 0.4, 0 },
                    { 0.4, 0.4, 0 },
                    { 0.4, 0.1, 0 },
                });
            _mesh.SetNormals(gpu, device,
                {
                    { 1, 0, 0 },
                    { 0, 1, 0 },
                    { 0, 0, 1 },
                    { 1, 1, 1 },
                });
            _mesh.SetUV(gpu, device,
                {
                    { 0.1, 0.1 },
                    { 0.5, 0.7 },
                    { 0.9, 0.1 },
                    { 0.9, 0.1 },
                });
        }

        Drawable::~Drawable()
        {
            _mesh.Destroy(_renderer->device());
            DestroyPipeline();
        }

        void Drawable::Draw(vk::CommandBuffer buffer)
        {
            buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, _pipeline);
            //buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, _pipelineLayout, 0, _descriptors, {});
            buffer.bindVertexBuffers(0, { _mesh.positions, _mesh.normals, _mesh.uv }, { 0, 0, 0 });
            buffer.bindIndexBuffer(_mesh.indices, 0, vk::IndexType::eUint16);
            buffer.pushConstants(_pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4), &_modelView);
            buffer.drawIndexed(_mesh.indexCount, 1, 0, 0, 0);
        }

        void Drawable::CreatePipeline(
            vk::ShaderModule vertexShader,
            vk::ShaderModule fragmentShader)
        {
            auto const viewport = _renderer->viewport();
            auto const vertices = VertexInput(sizeof(glm::vec3), 0);
            auto const normals = VertexInput(sizeof(glm::vec3), 1);
            auto const uv = VertexInput(sizeof(glm::vec2), 2, vk::Format::eR32G32Sfloat);

            std::vector<vk::VertexInputAttributeDescription> attributes;
            std::vector<vk::VertexInputBindingDescription> bindings;

            attributes.push_back(vertices.attribute);
            attributes.push_back(normals.attribute);
            attributes.push_back(uv.attribute);

            bindings.push_back(vertices.binding);
            bindings.push_back(normals.binding);
            bindings.push_back(uv.binding);

            auto const stages = ShaderStages(vertexShader, fragmentShader);

            auto const vertexInput = vk::PipelineVertexInputStateCreateInfo()
                .setVertexAttributeDescriptionCount(static_cast<uint32_t>(attributes.size()))
                .setPVertexAttributeDescriptions(attributes.data())
                .setVertexBindingDescriptionCount(static_cast<uint32_t>(bindings.size()))
                .setPVertexBindingDescriptions(bindings.data());

            auto const assembly = vk::PipelineInputAssemblyStateCreateInfo()
                .setTopology(vk::PrimitiveTopology::eTriangleList)
                .setPrimitiveRestartEnable(VK_FALSE);

            auto const scissor = vk::Rect2D(
                vk::Offset2D(0, 0),
                vk::Extent2D(static_cast<uint32_t>(viewport.width), static_cast<uint32_t>(viewport.height)));

            auto const viewportState = vk::PipelineViewportStateCreateInfo()
                .setPScissors(&scissor)
                .setScissorCount(1)
                .setViewportCount(1)
                .setPViewports(&viewport);

            auto const rasterization = vk::PipelineRasterizationStateCreateInfo()
                .setPolygonMode(vk::PolygonMode::eFill)
                .setCullMode(vk::CullModeFlagBits::eNone)
                .setFrontFace(vk::FrontFace::eClockwise);

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

            std::vector<vk::DynamicState> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eLineWidth };

            auto const constants = vk::PushConstantRange()
                .setOffset(0)
                .setSize(sizeof(glm::mat4))
                .setStageFlags(vk::ShaderStageFlagBits::eVertex);

            auto const dynamicState = vk::PipelineDynamicStateCreateInfo()
                .setDynamicStateCount(static_cast<uint32_t>(dynamicStates.size()))
                .setPDynamicStates(dynamicStates.data());

            auto const pipelineLayoutInfo = vk::PipelineLayoutCreateInfo()
                .setPushConstantRangeCount(1)
                .setPPushConstantRanges(&constants);

            CHECK_SUCCESS(_renderer->device().createPipelineLayout(&pipelineLayoutInfo, nullptr, &_pipelineLayout));

            auto const pipelineInfo = vk::GraphicsPipelineCreateInfo()
                .setStageCount(2)
                .setPStages(&stages.vertexStage)
                .setPVertexInputState(&vertexInput)
                .setPInputAssemblyState(&assembly)
                .setPViewportState(&viewportState)
                .setPRasterizationState(&rasterization)
                .setPMultisampleState(&multisampling)
                .setPColorBlendState(&blending)
                .setLayout(_pipelineLayout)
                .setPDynamicState(&dynamicState)
                .setRenderPass(_renderer->renderPass())
                .setSubpass(0);

            CHECK_SUCCESS(_renderer->device().createGraphicsPipelines(nullptr, 1, &pipelineInfo, nullptr, &_pipeline));
        }

        void Drawable::DestroyPipeline()
        {
            _renderer->device().destroyPipeline(_pipeline);
            _renderer->device().destroyPipelineLayout(_pipelineLayout);
        }
    }
}
