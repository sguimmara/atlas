#ifndef ATLAS_MESH_HPP
#define ATLAS_MESH_HPP

#include "AtlasGraphics.hpp"
#include "Node.hpp"
#include "Drawable.hpp"
#include "Renderer.hpp"
#include "Shader.hpp"
#include "MeshObject.hpp"
#include "Image.hpp"
#include "primitives/Tile.hpp"

namespace atlas
{
    namespace graphics
    {
        /**
        * @brief A Mesh is a Drawable with vertices organized in a mesh
        */
        class Mesh : public Drawable
        {
        public:
            Mesh(Renderer* renderer, MeshObject mesh);
            ~Mesh();

            /**
            * @brief Fills the command buffer with draw commands
            */
            void Draw(DrawContext context);

            void SendSignal(Signal signal);

        protected:
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
            vk::PolygonMode _currentPolygonMode;
            std::vector<vk::DescriptorSet> _descriptorSets;
            Renderer* _renderer;
            Shader _fragmentShader;
            Shader _vertexShader;

            MeshObject _mesh;
            Image _texture;
            glm::vec3 _color;
        };
    }
}

#endif