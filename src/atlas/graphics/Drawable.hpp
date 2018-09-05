#ifndef ATLAS_DRAWABLE_HPP
#define ATLAS_DRAWABLE_HPP

#include "AtlasGraphics.hpp"
#include "Node.hpp"
#include "Renderer.hpp"
#include "Shader.hpp"
#include "Mesh.hpp"
#include "Image.hpp"
#include "primitives/Tile.hpp"

namespace atlas
{
    namespace graphics
    {
        struct DrawContext
        {
            vk::CommandBuffer cmdBuffer;
            Transform viewMatrix;
            Transform projectionMatrix;
        };

        /**
        * @brief A GraphicsObject that can issue render commands
        */
        class Drawable : public Node
        {
        public:
            Drawable(Renderer* renderer);
            ~Drawable();

            /**
            * @brief Fills the command buffer with draw commands
            */
            void Draw(DrawContext context);

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

            struct MVP
            {
                glm::mat4 model;
                glm::mat4 view;
                glm::mat4 proj;
            };

            void CreateTexture();
            void CreateDescriptorPool(size_t swapchainSize);
            void CreateDescriptorSets(size_t swapchainSize);
            void DestroyDescriptorSets();
            void CreatePipeline(vk::ShaderModule, vk::ShaderModule);
            void DestroyPipeline();

            vk::Pipeline _pipeline;
            vk::PipelineLayout _pipelineLayout;
            vk::DescriptorPool _descriptorPool;
            vk::DescriptorSetLayout _descriptorSetLayout;
            std::vector<vk::DescriptorSet> _descriptorSets;
            Renderer* _renderer;
            Shader _fragmentShader;
            Shader _vertexShader;

            primitives::Tile _mesh;
            Image _texture;
        };
    }
}

#endif