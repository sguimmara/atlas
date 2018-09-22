#ifndef ATLAS_GRAPHICS_MATERIAL_HPP
#define ATLAS_GRAPHICS_MATERIAL_HPP

#include "AtlasGraphics.hpp"
#include "Shader.hpp"
#include <unordered_map>
#include <memory>

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
            std::string name;
            float lineWidth;
            vk::PrimitiveTopology topology;

            Shader vertexShader;
            Shader fragmentShader;

            vk::Pipeline pipeline;
            vk::PipelineLayout pipelineLayout;

            Material(std::string name, std::vector<Semantic> locations, std::vector<Descriptor> bindings, Shader vs, Shader fs, vk::PrimitiveTopology topology, vk::PolygonMode polygonMode = vk::PolygonMode::eFill);
            ~Material();

            static void CreateMaterials();
            static void DestroyMaterials();
            static std::shared_ptr<Material> Get(std::string name);

        private:
            static std::unordered_map<std::string, std::shared_ptr<Material>> _store;
            vk::DescriptorSetLayout _layout;
            std::vector<vk::DescriptorSet> _descriptorSets;

            void CreateDescriptorSet(size_t swapchainSize, std::vector<Descriptor> descriptors, vk::DescriptorPool pool);
        };
    }
}

#endif