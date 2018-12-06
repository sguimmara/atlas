#include "material.hpp"
#include "instance.hpp"

using namespace atlas::renderer;

Material::Material(const std::string& json)
{
    _pipeline = Pipeline::create(json);

    createDescriptorSet();
}

void Material::createDescriptorSet()
{
    _descriptorSet = Allocator::getDescriptorSet(_pipeline->descriptorSetLayout());
}

Material::Material(Pipeline* pipeline) :
    _pipeline(pipeline)
{
    createDescriptorSet();
}

Material::Material() :
    _pipeline(nullptr),
    _descriptorSet(nullptr)
{}

Material::Material(Material&& rhs) :
    _descriptorSet(rhs._descriptorSet),
    _pipeline(rhs._pipeline)
{
    rhs._pipeline = nullptr;
    rhs._descriptorSet = nullptr;
}

Material Material::instantiate() const
{
    return std::move(Material(_pipeline));
}

Material::~Material()
{
    if (_descriptorSet)
    {
        auto set = _descriptorSet;
        Allocator::free(_descriptorSet);
    }
}

inline vk::ImageLayout defaultLayout()
{
    return vk::ImageLayout::eShaderReadOnlyOptimal;
}

void Material::updateTexture(const uint32_t binding, const vk::ImageView& view, const vk::Sampler& sampler)
{
    auto const descImgInfo = vk::DescriptorImageInfo()
        .setSampler(sampler)
        .setImageLayout(defaultLayout())
        .setImageView(view);

    auto write = vk::WriteDescriptorSet()
        .setDstSet(_descriptorSet)
        .setDescriptorCount(1)
        .setDstBinding(binding)
        .setPImageInfo(&descImgInfo)
        .setDstArrayElement(0)
        .setDescriptorType(vk::DescriptorType::eCombinedImageSampler);

    Instance::device.updateDescriptorSets(1, &write, 0, nullptr);
}

void Material::setTexture(const std::string& name, const Texture* texture)
{
    uint32_t binding = _pipeline->getSamplerBinding(name);

    updateTexture(binding, texture->view(), texture->sampler());
}