#include "entity_data.hpp"
#include "instance.hpp"

using namespace atlas::renderer;

EntityData::EntityData()
{
    _buffer = Allocator::getBuffer(sizeof(EntityProperties), vk::BufferUsageFlagBits::eUniformBuffer);
    _set = Instance::createDescriptorSet(Pipeline::entityPropertyLayout());
}

EntityData::~EntityData()
{
    Instance::free(_set);
    Allocator::free(_buffer);
}

void EntityData::update(EntityProperties props)
{
    Allocator::write(_buffer, &props, sizeof(EntityProperties));

    auto const bufferInfo = vk::DescriptorBufferInfo()
        .setBuffer(_buffer)
        .setOffset(0)
        .setRange(sizeof(EntityProperties));

    auto const write = vk::WriteDescriptorSet()
        .setDescriptorCount(1)
        .setDescriptorType(vk::DescriptorType::eUniformBuffer)
        .setDstBinding(1)
        .setDstSet(_set)
        .setPBufferInfo(&bufferInfo);

    Instance::device.updateDescriptorSets(1, &write, 0, nullptr);
}
