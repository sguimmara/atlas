#ifndef ATLAS_GRAPHICS_MATERIAL_HPP
#define ATLAS_GRAPHICS_MATERIAL_HPP

#include "AtlasGraphics.hpp"
#include "Shader.hpp"

namespace atlas
{
    namespace graphics
    {
        enum Semantic
        {
            Position,
            Normal,
            Color,
            TexCoord
        };

        struct Descriptor
        {
            vk::DescriptorType type = vk::DescriptorType::eCombinedImageSampler;
            vk::ShaderStageFlags flags = vk::ShaderStageFlagBits::eFragment;

            vk::ImageView view;
        };

        struct MVP
        {
            glm::mat4 model;
            glm::mat4 view;
            glm::mat4 proj;
        };

        struct Material
        {
            std::vector<Semantic> locations;
            std::vector<Descriptor> bindings;

            float lineWidth;

            Shader vertexShader;
            Shader fragmentShader;

            vk::Pipeline pipeline;
            vk::PipelineLayout pipelineLayout;

            Material();
            Material(std::vector<Semantic> locations, std::vector<Descriptor> bindings, Shader vs, Shader fs, vk::PrimitiveTopology topology);

        private:
            vk::DescriptorSetLayout _layout;
            std::vector<vk::DescriptorSet> _descriptorSets;

            void CreateDescriptorSet(size_t swapchainSize, std::vector<Descriptor> descriptors, vk::DescriptorPool pool);
        };
    }
}

#endif