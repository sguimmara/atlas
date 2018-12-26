#include "material.hpp"
#include "instance.hpp"

using namespace atlas::renderer;

Material::Material(const std::string& json)
{
    _pipeline = Pipeline::create(json);

    createDescriptorSet();
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
    _pipeline(rhs._pipeline),
    _propertyBuffer(std::move(rhs._propertyBuffer)),
    _descriptorSet(rhs._descriptorSet)
{
    rhs._pipeline = nullptr;
    rhs._descriptorSet = nullptr;
    rhs._propertyBuffer = nullptr;
}

Material Material::instantiate() const
{
    return Material(_pipeline);
}

Material::~Material()
{
    if (_descriptorSet)
    {
        Allocator::free(_descriptorSet);
    }
}

void Material::update() noexcept
{
    _propertyBuffer->update(&_properties);
}

void Material::createDescriptorSet()
{
    spdlog::get("instance")->trace("createDescriptorSet");
    _descriptorSet = Allocator::getDescriptorSet(_pipeline->descriptorSetLayout());
    spdlog::get("instance")->trace("uniform buffer");
    _propertyBuffer = std::make_unique<UniformBuffer>(sizeof(MaterialProperties), 2, _descriptorSet);
    spdlog::get("instance")->trace("update");
    _propertyBuffer->update(&_properties);
    spdlog::get("instance")->trace("update done");
}

std::shared_ptr<Material> Material::create(const std::string name)
{
    assert(Pipeline::exists(name));
    return std::make_shared<Material>(Pipeline::get(name));
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