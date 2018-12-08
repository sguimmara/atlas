#include "mesh.hpp"

using namespace atlas::core;
using namespace atlas::renderer;

Mesh::Mesh()
{}

Mesh::Mesh(Mesh&& rhs) :
    _buffer(rhs._buffer),
    _vertexOffset(rhs._vertexOffset),
    _indexOffset(rhs._indexOffset),
    _indexCount(rhs._indexCount),
    _vertexCount(rhs._vertexCount),
    _bounds(rhs._bounds)
{
    rhs._buffer = nullptr;
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint16_t> indices) :
    _indexCount((uint32_t)indices.size()),
    _vertexCount((uint32_t)vertices.size())
{
    size_t vertexSize = vertices.size() * sizeof(Vertex);
    size_t indexSize = indices.size() * sizeof(uint16_t);

    _buffer = Allocator::getBuffer(vertexSize + indexSize, vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eVertexBuffer);

    Allocator::write(_buffer, vertices.data(), vertexSize);
    Allocator::write(_buffer, indices.data(), indexSize, vertexSize);

    _vertexOffset = 0;
    _indexOffset = (vk::DeviceSize)vertexSize;
    _bounds = Bounds(vertices);
}

Mesh::~Mesh()
{
    if (_buffer)
    {
        Allocator::free(_buffer);
    }
}
