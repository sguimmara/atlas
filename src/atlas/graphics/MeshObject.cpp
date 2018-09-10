#include "MeshObject.hpp"
#include "Renderer.hpp"

namespace atlas
{
    namespace graphics
    {
        void MeshObject::Destroy()
        {
            Renderer::device.destroyBuffer(indices);
            Renderer::device.destroyBuffer(buffer);
            Renderer::device.freeMemory(indicesMemory);
            Renderer::device.freeMemory(bufferMemory);
        }

        MeshObject::MeshObject(uint32_t vertexCount) : vertexCount(vertexCount)
        {
            _vertices.resize(vertexCount);
        }

        void MeshObject::SetPositions(std::vector<glm::vec3> &data)
        {
            if (data.size() != _vertices.size())
            {
                throw std::runtime_error("invalid size of buffer");
            }

            for (size_t i = 0; i < _vertices.size(); ++i)
            {
                _vertices[i].position = data[i];
            }
        }

        void MeshObject::SetNormals(std::vector<glm::vec3>& data)
        {
            if (data.size() != _vertices.size())
            {
                throw std::runtime_error("invalid size of buffer");
            }

            for (size_t i = 0; i < _vertices.size(); ++i)
            {
                _vertices[i].normal = data[i];
            }
        }

        void MeshObject::SetUV(std::vector<glm::vec2>& data)
        {
            if (data.size() != _vertices.size())
            {
                throw std::runtime_error("invalid size of buffer");
            }

            for (size_t i = 0; i < _vertices.size(); ++i)
            {
                _vertices[i].uv = glm::packHalf2x16(data[i]);
            }
        }

        void MeshObject::Apply()
        {
            CreateBuffer(_vertices.data(), _vertices.size() * sizeof(Vertex), vk::BufferUsageFlagBits::eVertexBuffer, buffer, bufferMemory);
        }

        void MeshObject::SetIndices(std::vector<uint16_t>& data)
        {
            indexCount = static_cast<uint32_t>(data.size());
            CreateBuffer(data.data(), data.size() * sizeof(uint16_t), vk::BufferUsageFlagBits::eIndexBuffer, indices, indicesMemory);
        }

        uint32_t FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
        {
            vk::PhysicalDeviceMemoryProperties memory = Renderer::gpu.getMemoryProperties();

            for (uint32_t i = 0; i < memory.memoryTypeCount; i++)
            {
                if ((typeFilter & (1 << i)) && (memory.memoryTypes[i].propertyFlags & properties) == properties)
                {
                    return i;
                }
            }
            
            throw std::runtime_error("no memory type found");
        }

        void MeshObject::CreateBuffer(void* data, size_t size, vk::BufferUsageFlags usage,
            vk::Buffer& buf, vk::DeviceMemory& memory)
        {
            auto const info = vk::BufferCreateInfo()
                .setSize(size)
                .setUsage(usage)
                .setSharingMode(vk::SharingMode::eExclusive);

            auto result = Renderer::device.createBuffer(&info, nullptr, &buf);
            CHECK_SUCCESS(result);

            auto const reqs = Renderer::device.getBufferMemoryRequirements(buf);

            auto const alloc = vk::MemoryAllocateInfo()
                .setAllocationSize(reqs.size)
                .setMemoryTypeIndex(
                    FindMemoryType(reqs.memoryTypeBits,
                        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));

            void* tmp;
            CHECK_SUCCESS(Renderer::device.allocateMemory(&alloc, nullptr, &memory));
            Renderer::device.bindBufferMemory(buf, memory, 0);
            CHECK_SUCCESS(Renderer::device.mapMemory(memory, 0, info.size, vk::MemoryMapFlags(), &tmp));
            memcpy(tmp, data, static_cast<size_t>(info.size));
            Renderer::device.unmapMemory(memory);
        }
    }
}
