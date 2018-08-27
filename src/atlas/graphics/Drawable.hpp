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

        /* A drawable is a node that can issue render commands */
        class Drawable : public GraphicsObject
        {
        public:
            Drawable(Renderer* renderer);
            ~Drawable();

            virtual void UpdateCommandBuffers();
            virtual vk::CommandBuffer GetCommandBuffer(uint32_t imageIndex);

        protected:
            void CreatePipeline(vk::Viewport, vk::ShaderModule, vk::ShaderModule);
            void DestroyPipeline();
            void CreateCommandBuffers();

            std::vector<vk::CommandBuffer> _commandBuffers;
            vk::Viewport _viewport;
            vk::Pipeline _pipeline;
            vk::PipelineLayout _pipelineLayout;
            Renderer* _renderer;
            Shader _fragmentShader;
            Shader _vertexShader;
        };
    }
}

#endif