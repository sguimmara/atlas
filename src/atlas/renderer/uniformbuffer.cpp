#include "uniformbuffer.hpp"
#include "instance.hpp"

using namespace atlas::renderer;

UniformBuffer::UniformBuffer(UniformBuffer&& rhs) :
    _size(rhs._size),
    _binding(rhs._binding),
    _buffer(rhs._buffer),
    _descriptorSet(rhs._descriptorSet)
{
    rhs._buffer = nullptr;
    rhs._descriptorSet = nullptr;
}

UniformBuffer::UniformBuffer(size_t size, uint32_t binding, vk::DescriptorSetLayout layout) :
    _size((uint32_t)size),
    _binding(binding)
{
    _descriptorSet = Allocator::getDescriptorSet(layout);
    _buffer = Allocator::getBuffer((uint32_t)size, vk::BufferUsageFlagBits::eUniformBuffer);
}

UniformBuffer::~UniformBuffer()
{
    if (_buffer)
    {
        Allocator::free(_buffer);
    }
    if (_descriptorSet)
    {
        Allocator::free(_descriptorSet);
    }
}

void UniformBuffer::update(void * data)
{
    Allocator::write(_buffer, data, _size, 0);

    auto const bufferInfo = vk::DescriptorBufferInfo()
        .setBuffer(_buffer)
        .setOffset(0)
        .setRange(sizeof(GlobalProperties));

    auto const write = vk::WriteDescriptorSet()
        .setDescriptorCount(1)
        .setDescriptorType(vk::DescriptorType::eUniformBuffer)
        .setDstBinding(_binding)
        .setDstSet(_descriptorSet)
        .setPBufferInfo(&bufferInfo);

    Instance::device.updateDescriptorSets(1, &write, 0, nullptr);
}
