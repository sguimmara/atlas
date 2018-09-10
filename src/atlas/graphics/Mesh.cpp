#include "RenderingOptions.hpp"
#include "Mesh.hpp"
#include "Time.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace atlas
{
    namespace graphics
    {
        Mesh::Mesh(Renderer* renderer, MeshObject mesh) :
            _renderer(renderer),
            _mesh(mesh),
            _fragmentShader(Shader::Get("tile.frag", renderer->device())),
            _vertexShader(Shader::Get("tile.vert", renderer->device())),
            Drawable()
        {
            _flags |= (int)NodeFlags::Drawable;
            CreateTexture();
            CreateDescriptorPool(renderer->swapchainSize());
            CreateDescriptorSets(renderer->swapchainSize());
            CreatePipeline(_vertexShader.module, _fragmentShader.module);
        }

        Mesh::~Mesh()
        {
            _mesh.Destroy(_renderer->device());
            _texture.Destroy(_renderer->device());
            DestroyPipeline();
            DestroyDescriptorSets();
        }

        void Mesh::Draw(DrawContext context)
        {
            _localTransform = glm::rotate(_localTransform, static_cast<float>(Time::dt * 0.4), glm::vec3(0, 1, 0));

            if (_currentPolygonMode != RenderingOptions::PolygonMode)
            {
                DestroyPipeline();
                CreatePipeline(_vertexShader.module, _fragmentShader.module);
            }
            
            struct MVP mvp;
            mvp.model = _transform;
            mvp.view = context.viewMatrix;
            mvp.proj = context.projectionMatrix;

            vk::CommandBuffer buffer = context.cmdBuffer;
            buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, _pipeline);
            buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, _pipelineLayout, 0, 1, _descriptorSets.data(), 0, nullptr);
            buffer.bindVertexBuffers(0, { _mesh.buffer }, { 0 });
            buffer.bindIndexBuffer(_mesh.indices, 0, vk::IndexType::eUint16);
            buffer.pushConstants(_pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(MVP), &mvp);
            buffer.pushConstants(_pipelineLayout, vk::ShaderStageFlagBits::eVertex, sizeof(MVP), sizeof(glm::vec3), &_color);
            buffer.drawIndexed(_mesh.indexCount, 1, 0, 0, 0);
        }

        void Mesh::SendSignal(Signal signal)
        {
            if (signal == Signal::WindowResized)
            {
                DestroyPipeline();
                CreatePipeline(_vertexShader.module, _fragmentShader.module);
            }
        }

        void Mesh::CreateDescriptorPool(size_t swapchainSize)
        {
            auto const sizes = vk::DescriptorPoolSize()
                .setDescriptorCount(static_cast<uint32_t>(swapchainSize));

            auto const info = vk::DescriptorPoolCreateInfo()
                .setPoolSizeCount(1)
                .setPPoolSizes(&sizes)
                .setMaxSets(static_cast<uint32_t>(swapchainSize));

            CHECK_SUCCESS(_renderer->device().createDescriptorPool(&info, nullptr, &_descriptorPool));
        }

        void Mesh::CreateDescriptorSets(size_t swapchainSize)
        {
            auto const texture0 = vk::DescriptorSetLayoutBinding()
                .setBinding(0)
                .setDescriptorCount(1)
                .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                .setStageFlags(vk::ShaderStageFlagBits::eFragment)
                .setPImmutableSamplers(nullptr);

            std::vector<vk::DescriptorSetLayoutBinding> bindings{ texture0 };

            auto const layoutInfo = vk::DescriptorSetLayoutCreateInfo()
                .setBindingCount(static_cast<uint32_t>(bindings.size()))
                .setPBindings(bindings.data());

            _descriptorSetLayout;
            CHECK_SUCCESS(_renderer->device().createDescriptorSetLayout(&layoutInfo, nullptr, &_descriptorSetLayout));

            std::vector<vk::DescriptorSetLayout> layouts;
            for (size_t i = 0; i < swapchainSize; i++)
            {
                layouts.push_back(_descriptorSetLayout);
            }
            _descriptorSets.resize(swapchainSize);

            auto const allocInfo = vk::DescriptorSetAllocateInfo()
                .setDescriptorPool(_descriptorPool)
                .setDescriptorSetCount(static_cast<uint32_t>(swapchainSize))
                .setPSetLayouts(layouts.data());

            _renderer->device().allocateDescriptorSets(&allocInfo, _descriptorSets.data());
            
            auto const imageInfo = vk::DescriptorImageInfo()
                .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                .setImageView(_texture.view)
                .setSampler(_texture.sampler);

            std::vector<vk::WriteDescriptorSet> writes;
            writes.resize(_descriptorSets.size());

            for (size_t i = 0; i < _descriptorSets.size(); i++)
            {
                auto const write = vk::WriteDescriptorSet()
                    .setDstSet(_descriptorSets[i])
                    .setDescriptorCount(1)
                    .setPImageInfo(&imageInfo)
                    .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                    .setDstArrayElement(0)
                    .setDstBinding(0);
                writes[i] = write;
            }
            
            _renderer->device().updateDescriptorSets(static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
        }

        void Mesh::DestroyDescriptorSets()
        {
            _renderer->device().destroyDescriptorSetLayout(_descriptorSetLayout);
            _renderer->device().destroyDescriptorPool(_descriptorPool);
        }

        void Mesh::CreateTexture()
        {
            _texture = Image::FromFile(_renderer, "C:/Users/sguimmara/Documents/work/c++/atlas/images/uv_grid.jpg");
        }

        void Mesh::CreatePipeline(
            vk::ShaderModule vertexShader,
            vk::ShaderModule fragmentShader)
        {
            auto const viewport = _renderer->viewport();

            auto const binding = vk::VertexInputBindingDescription()
                .setBinding(0)
                .setStride(sizeof(Vertex))
                .setInputRate(vk::VertexInputRate::eVertex);

            auto const posAttrib = vk::VertexInputAttributeDescription()
                .setBinding(0)
                .setLocation(0)
                .setFormat(vk::Format::eR32G32B32Sfloat)
                .setOffset(offsetof(Vertex, position));

            auto const normAttrib = vk::VertexInputAttributeDescription()
                .setBinding(0)
                .setLocation(1)
                .setFormat(vk::Format::eR32G32B32Sfloat)
                .setOffset(offsetof(Vertex, normal));

            auto const uvAttrib = vk::VertexInputAttributeDescription()
                .setBinding(0)
                .setLocation(2)
                .setFormat(vk::Format::eR16G16Sfloat)
                .setOffset(offsetof(Vertex, uv));

            std::vector<vk::VertexInputAttributeDescription> attributes;
            std::vector<vk::VertexInputBindingDescription> bindings;

            attributes.push_back(posAttrib);
            attributes.push_back(normAttrib);
            attributes.push_back(uvAttrib);

            bindings.push_back(binding);

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
                .setPolygonMode(RenderingOptions::PolygonMode)
                .setCullMode(vk::CullModeFlagBits::eBack)
                .setFrontFace(vk::FrontFace::eClockwise);

            _currentPolygonMode = RenderingOptions::PolygonMode;

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

            auto const constantRange = vk::PushConstantRange()
                .setOffset(0)
                .setSize(sizeof(MVP) + sizeof(glm::vec3))
                .setStageFlags(vk::ShaderStageFlagBits::eVertex);

            auto const dynamicState = vk::PipelineDynamicStateCreateInfo()
                .setDynamicStateCount(static_cast<uint32_t>(dynamicStates.size()))
                .setPDynamicStates(dynamicStates.data());

            auto const pipelineLayoutInfo = vk::PipelineLayoutCreateInfo()
                .setSetLayoutCount(1)
                .setPSetLayouts(&_descriptorSetLayout)
                .setPushConstantRangeCount(1)
                .setPPushConstantRanges(&constantRange);

            CHECK_SUCCESS(_renderer->device().createPipelineLayout(&pipelineLayoutInfo, nullptr, &_pipelineLayout));

            auto const depthStencil = vk::PipelineDepthStencilStateCreateInfo()
                .setDepthTestEnable(VK_TRUE)
                .setDepthWriteEnable(VK_TRUE)
                .setDepthCompareOp(vk::CompareOp::eLess)
                .setStencilTestEnable(VK_FALSE);

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
                .setPDepthStencilState(&depthStencil)
                .setSubpass(0);

            CHECK_SUCCESS(_renderer->device().createGraphicsPipelines(nullptr, 1, &pipelineInfo, nullptr, &_pipeline));
        }

        void Mesh::DestroyPipeline()
        {
            _renderer->device().destroyPipeline(_pipeline);
            _renderer->device().destroyPipelineLayout(_pipelineLayout);
        }
    }
}
