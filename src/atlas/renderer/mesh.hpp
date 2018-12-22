#pragma once
#ifndef ATLAS_RENDERER_MESH_HPP
#define ATLAS_RENDERER_MESH_HPP

#include "common.hpp"
#include "vertex.hpp"
#include "allocator.hpp"
#include "atlas/core/bounds.hpp"
#include <vector>

namespace atlas::renderer
{
    class Mesh
    {
    public:
        Mesh();
        Mesh(Mesh&& rhs);
        Mesh(std::vector<Vertex>, std::vector<uint16_t>);
        ~Mesh();

        inline core::Bounds bounds() const noexcept { return _bounds; }
        inline vk::Buffer buffer() const noexcept { return _buffer; }

        inline vk::DeviceSize vertexOffset() const noexcept { return _vertexOffset; }
        inline vk::DeviceSize indexOffset() const noexcept{ return _indexOffset; }

        inline uint32_t indexCount() const noexcept { return _indexCount; }
        inline uint32_t vertexCount() const noexcept { return _vertexCount; }

    private:

        vk::Buffer _buffer;
        vk::DeviceSize _vertexOffset;
        vk::DeviceSize _indexOffset;
        uint32_t _indexCount;
        uint32_t _vertexCount;
        core::Bounds _bounds;
    };
}

#endif // ATLAS_RENDERER_MESH_HPP