#include "Drawable.hpp"

namespace atlas
{
    namespace graphics
    {
        Drawable::Drawable(Renderer* renderer) :
            _renderer(renderer),
            _fragmentShader(Shader("unlit-frag", "./shaders/blank.frag.spv", renderer->device())),
            _vertexShader(Shader("unlit-vert", "./shaders/blank.vert.spv", renderer->device())),
            GraphicsObject()
        {
            CreatePipeline(vk::Viewport(0, 0, 100, 100), _vertexShader.shaderModule(), _fragmentShader.shaderModule());
            CreateCommandBuffers();
        }

        Drawable::~Drawable()
        {
            DestroyPipeline();
        }

        vk::CommandBuffer Drawable::GetCommandBuffer(uint32_t imageIndex)
        {
            return _commandBuffers[imageIndex];
        }

        void Drawable::CreatePipeline(
            vk::Viewport viewport,
            vk::ShaderModule vertexShader,
            vk::ShaderModule fragmentShader)
        {
            _viewport = viewport;
            auto const vertices = VertexInput(sizeof(glm::vec3), 0);
            auto const colors = VertexInput(sizeof(glm::vec3), 1);

            std::vector<vk::VertexInputAttributeDescription> attributes;
            std::vector<vk::VertexInputBindingDescription> bindings;

            attributes.push_back(vertices.attribute);
            attributes.push_back(colors.attribute);

            bindings.push_back(vertices.binding);
            bindings.push_back(colors.binding);

            auto const stages = ShaderStages(vertexShader, fragmentShader);

            auto const vertexInput = vk::PipelineVertexInputStateCreateInfo();
            //.setVertexAttributeDescriptionCount(static_cast<uint32_t>(attributes.size()))
            //.setPVertexAttributeDescriptions(attributes.data())
            //.setVertexBindingDescriptionCount(static_cast<uint32_t>(bindings.size()))
            //.setPVertexBindingDescriptions(bindings.data());

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

            vk::Result result;

            result = _renderer->device().createPipelineLayout(&pipelineLayoutInfo, nullptr, &_pipelineLayout);
            VERIFY(result == vk::Result::eSuccess);

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
                .setRenderPass(_renderer->framebuffer()->renderPass())
                .setSubpass(0);

            result = _renderer->device().createGraphicsPipelines(nullptr, 1, &pipelineInfo, nullptr, &_pipeline);
            VERIFY(result == vk::Result::eSuccess);
        }

        void Drawable::DestroyPipeline()
        {
            _renderer->device().destroyPipeline(_pipeline);
            _renderer->device().destroyPipelineLayout(_pipelineLayout);
        }

        void Drawable::CreateCommandBuffers()
        {
            _commandBuffers.resize(_renderer->framebuffer()->imageCount());

            auto const allocInfo = vk::CommandBufferAllocateInfo()
                .setCommandBufferCount(static_cast<uint32_t>(_commandBuffers.size()))
                .setCommandPool(_renderer->pool())
                .setLevel(vk::CommandBufferLevel::ePrimary);

            auto result = _renderer->device().allocateCommandBuffers(&allocInfo, _commandBuffers.data());
            VERIFY(result == vk::Result::eSuccess);

            UpdateCommandBuffers();
        }

        void Drawable::UpdateCommandBuffers()
        {
            for (uint32_t i = 0; i < _commandBuffers.size(); i++)
            {
                auto buffer = _commandBuffers[i];

                auto const begin = vk::CommandBufferBeginInfo()
                    .setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse);

                auto result = buffer.begin(&begin);
                VERIFY(result == vk::Result::eSuccess);
                {
                    auto const clearColor = vk::ClearColorValue()
                        .setFloat32({ 1.0f,1.0f,1.0f,1.0f });

                    auto const clear = vk::ClearValue()
                        .setColor(clearColor);

                    auto const renderPassInfo = vk::RenderPassBeginInfo()
                        .setRenderPass(_renderer->framebuffer()->renderPass())
                        .setFramebuffer(_renderer->framebuffer()->framebuffer(i))
                        .setRenderArea(vk::Rect2D(vk::Offset2D(0, 0), _renderer->extent()))
                        .setClearValueCount(1)
                        .setPClearValues(&clear);

                    buffer.setViewport(0, 1, &_viewport);
                    buffer.beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);
                    {
                        //glm::mat4 mvp(1.0);
                        //buffer.pushConstants(_pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4), &mvp);
                        //buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, _pipeline);
                        //buffer.draw(1, 1, 0, 0);
                    }
                    buffer.endRenderPass();
                }
                buffer.end();
            }
        }
    }
}
