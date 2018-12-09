#ifndef ATLAS_RENDERER_MATERIAL_HPP
#define ATLAS_RENDERER_MATERIAL_HPP

#include "common.hpp"
#include "pipeline.hpp"
#include "texture.hpp"
#include "materialproperties.hpp"
#include "uniformbuffer.hpp"

namespace atlas::renderer
{
    // the Material represents a particular instance of a pipeline.
    class Material
    {
    public:
        // constructs an empty material
        Material();

        // constructs a material from a JSON description
        Material(const std::string& description);

        // constructs a material from the given pipeline.
        Material(Pipeline*);

        // constructs a material from transferring ownership of resources from
        // the specified material. The old material becomes empty.
        Material(Material&& rhs);

        ~Material();

        // returns a material with its own copy of the resources.
        Material instantiate() const;

        void setTexture(const std::string & name, const Texture * texture);

        inline Pipeline* pipeline() const noexcept { return _pipeline; }

        inline vk::DescriptorSet descriptorSet() const noexcept { return _descriptorSet; }

    private:
        Pipeline* _pipeline;
        MaterialProperties _properties;
        std::unique_ptr<UniformBuffer> _propertyBuffer;
        vk::DescriptorSet _descriptorSet;

        void createDescriptorSet();
        void updateTexture(const uint32_t binding, const vk::ImageView & view, const vk::Sampler & sampler);
    };
}

#endif