#include "Mesh.hpp"

namespace atlas
{
    namespace graphics
    {
        void Mesh::Destroy(vk::Device device)
        {
            device.destroyBuffer(indices);
            device.destroyBuffer(buffer);
            device.freeMemory(indicesMemory);
            device.freeMemory(bufferMemory);
        }

        Mesh::Mesh(uint32_t vertexCount)
        {
            _vertices.resize(vertexCount);
        }

        void Mesh::SetPositions(std::vector<glm::vec3> &data)
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

        void Mesh::SetNormals(std::vector<glm::vec3>& data)
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

        void Mesh::SetUV(std::vector<glm::vec2>& data)
        {
            if (data.size() != _vertices.size())
            {
                throw std::runtime_error("invalid size of buffer");
            }

            for (size_t i = 0; i < _vertices.size(); ++i)
            {
                _vertices[i].uv = data[i];
            }
        }

        void Mesh::Apply(vk::PhysicalDevice gpu, vk::Device device)
        {
            CreateBuffer(gpu, device, _vertices.data(), _vertices.size() * sizeof(Vertex), vk::BufferUsageFlagBits::eVertexBuffer, buffer, bufferMemory);
        }

        void Mesh::SetIndices(vk::PhysicalDevice gpu, vk::Device device, std::vector<uint16_t>& data)
        {
            indexCount = static_cast<uint32_t>(data.size());
            CreateBuffer(gpu, device, data.data(), data.size() * sizeof(uint16_t), vk::BufferUsageFlagBits::eIndexBuffer, indices, indicesMemory);
        }

        uint32_t FindMemoryType(vk::PhysicalDevice gpu, uint32_t typeFilter, vk::MemoryPropertyFlags properties)
        {
            vk::PhysicalDeviceMemoryProperties memory = gpu.getMemoryProperties();

            for (uint32_t i = 0; i < memory.memoryTypeCount; i++)
            {
                if ((typeFilter & (1 << i)) && (memory.memoryTypes[i].propertyFlags & properties) == properties)
                {
                    return i;
                }
            }
            
            throw std::runtime_error("no memory type found");
        }

        void Mesh::CreateBuffer(vk::PhysicalDevice gpu, vk::Device device,
            void* data, size_t size, vk::BufferUsageFlags usage,
            vk::Buffer& buf, vk::DeviceMemory& memory)
        {
            auto const info = vk::BufferCreateInfo()
                .setSize(size)
                .setUsage(usage)
                .setSharingMode(vk::SharingMode::eExclusive);

            auto result = device.createBuffer(&info, nullptr, &buf);
            CHECK_SUCCESS(result);

            auto const reqs = device.getBufferMemoryRequirements(buf);

            auto const alloc = vk::MemoryAllocateInfo()
                .setAllocationSize(reqs.size)
                .setMemoryTypeIndex(
                    FindMemoryType(gpu, reqs.memoryTypeBits,
                        vk::MemoryPropertyFlagBits::eHostVisible
                        | vk::MemoryPropertyFlagBits::eHostCoherent));

            void* tmp;
            CHECK_SUCCESS(device.allocateMemory(&alloc, nullptr, &memory));
            device.bindBufferMemory(buf, memory, 0);
            CHECK_SUCCESS(device.mapMemory(memory, 0, info.size, vk::MemoryMapFlags(), &tmp));
            memcpy(tmp, data, static_cast<size_t>(info.size));
            device.unmapMemory(memory);
        }
    }
}
