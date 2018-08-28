#ifndef ATLAS_DRAWABLE_HPP
#define ATLAS_DRAWABLE_HPP

#include "AtlasGraphics.hpp"
#include "GraphicsObject.hpp"
#include "Renderer.hpp"
#include "Shader.hpp"

namespace atlas
{
    namespace graphics
    {
        /**
        * @brief A GraphicsObject that can issue render commands
        */
        class Drawable : public GraphicsObject
        {
        public:
            Drawable(Renderer* renderer);
            ~Drawable();

            /**
            * @brief Fills the command buffer with draw commands
            */
            void Draw(vk::CommandBuffer buffer);

        protected:
            struct VertexInput
            {
                vk::VertexInputBindingDescription binding;
                vk::VertexInputAttributeDescription attribute;

                VertexInput(uint32_t stride, uint32_t location, vk::Format format = vk::Format::eR32G32B32Sfloat)
                {
                    binding = vk::VertexInputBindingDescription()
                        .setBinding(location)
                        .setStride(stride)
                        .setInputRate(vk::VertexInputRate::eVertex);

                    attribute = vk::VertexInputAttributeDescription()
                        .setBinding(location)
                        .setLocation(location)
                        .setFormat(format)
                        .setOffset(0);
                }
            };

            struct ShaderStages
            {
                vk::PipelineShaderStageCreateInfo vertexStage;
                vk::PipelineShaderStageCreateInfo fragmentStage;

                ShaderStages(vk::ShaderModule vertex, vk::ShaderModule fragment)
                {
                    vertexStage = vk::PipelineShaderStageCreateInfo()
                        .setStage(vk::ShaderStageFlagBits::eVertex)
                        .setModule(vertex)
                        .setPName("main");

                    fragmentStage = vk::PipelineShaderStageCreateInfo()
                        .setStage(vk::ShaderStageFlagBits::eFragment)
                        .setModule(fragment)
                        .setPName("main");
                }
            };

            void CreatePipeline(vk::ShaderModule, vk::ShaderModule);
            void DestroyPipeline();

            vk::Pipeline _pipeline;
            vk::PipelineLayout _pipelineLayout;
            std::vector<vk::DescriptorSet> _descriptors;
            Renderer* _renderer;
            Shader _fragmentShader;
            Shader _vertexShader;

            Mesh _mesh;
        };
    }
}

#endif