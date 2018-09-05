#include "Drawable.hpp"
#include <glm/gtc/matrix_transform.hpp>

#define SHADER_DIR "C:/Users/sguimmara/Documents/work/c++/atlas/build/msvc/bin/shaders/"

namespace atlas
{
    namespace graphics
    {
        Drawable::Drawable(Renderer* renderer) :
            _renderer(renderer),
            _mesh(renderer->gpu(), renderer->device(), 32, glm::vec2(0, 0), glm::vec2(PI, 2*PI)),
            _fragmentShader(Shader("unlit-frag", SHADER_DIR "unlit.frag.spv", renderer->device())),
            _vertexShader(Shader("unlit-vert", SHADER_DIR "unlit.vert.spv", renderer->device())),
            Node()
        {
            CreateTexture();
            CreateDescriptorPool(renderer->swapchainSize());
            CreateDescriptorSets(renderer->swapchainSize());
            CreatePipeline(_vertexShader.shaderModule(), _fragmentShader.shaderModule());
        }

        Drawable::~Drawable()
        {
            _mesh.Destroy(_renderer->device());
            _texture.Destroy(_renderer->device());
            DestroyPipeline();
            DestroyDescriptorSets();
        }

        static int occur = 0;

        void Drawable::Draw(DrawContext context)
        {
            occur++;
            struct MVP mvp;
            mvp.model = glm::rotate(_transform, occur * 0.005f, glm::vec3(0, 1, 0));
            mvp.view = context.viewMatrix;
            mvp.proj = context.projectionMatrix;
            glm::vec3 color = { 1, 1, 1 };

            vk::CommandBuffer buffer = context.cmdBuffer;
            buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, _pipeline);
            buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, _pipelineLayout, 0, 1, _descriptorSets.data(), 0, nullptr);
            buffer.bindVertexBuffers(0, { _mesh.positions, _mesh.normals, _mesh.uv }, { 0, 0, 0 });
            buffer.bindIndexBuffer(_mesh.indices, 0, vk::IndexType::eUint16);
            buffer.pushConstants(_pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(MVP), &mvp);
            buffer.pushConstants(_pipelineLayout, vk::ShaderStageFlagBits::eVertex, sizeof(MVP), sizeof(glm::vec3), &color);
            buffer.drawIndexed(_mesh.indexCount, 1, 0, 0, 0);
        }

        void Drawable::CreateDescriptorPool(size_t swapchainSize)
        {
            auto const sizes = vk::DescriptorPoolSize()
                .setDescriptorCount(static_cast<uint32_t>(swapchainSize));

            auto const info = vk::DescriptorPoolCreateInfo()
                .setPoolSizeCount(1)
                .setPPoolSizes(&sizes)
                .setMaxSets(static_cast<uint32_t>(swapchainSize));

            CHECK_SUCCESS(_renderer->device().createDescriptorPool(&info, nullptr, &_descriptorPool));
        }

        void Drawable::CreateDescriptorSets(size_t swapchainSize)
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

        void Drawable::DestroyDescriptorSets()
        {
            _renderer->device().destroyDescriptorSetLayout(_descriptorSetLayout);
            _renderer->device().destroyDescriptorPool(_descriptorPool);
        }

        void Drawable::CreateTexture()
        {
            _texture = Image::FromFile(_renderer, "C:/Users/sguimmara/Documents/work/c++/atlas/images/equirectangular.jpg");
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
                .setCullMode(vk::CullModeFlagBits::eBack)
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
