#ifndef ATLAS_RENDERER_UNIFORMBUFFER_HPP
#define ATLAS_RENDERER_UNIFORMBUFFER_HPP

#include "common.hpp"

namespace atlas::renderer
{
    class UniformBuffer
    {
    public:
        UniformBuffer(UniformBuffer&) = delete;
        UniformBuffer(UniformBuffer&& rhs);
        UniformBuffer(size_t size, uint32_t binding, vk::DescriptorSetLayout);
        ~UniformBuffer();

        // update the underlying buffer with the provide data.
        void update(void* data);

        inline vk::DescriptorSet descriptorSet() const noexcept { return _descriptorSet; }

    private:
        uint32_t _size;
        uint32_t _binding;
        vk::Buffer _buffer;
        vk::DescriptorSet _descriptorSet;
    };
}

#endif