#include "Mesh.hpp"

namespace atlas
{
    namespace graphics
    {
        void Mesh::Destroy(vk::Device device)
        {
            device.destroyBuffer(indices);
            device.destroyBuffer(positions);
            device.destroyBuffer(normals);
            device.destroyBuffer(uv);
            device.freeMemory(indicesMemory);
            device.freeMemory(positionsMemory);
            device.freeMemory(normalsMemory);
            device.freeMemory(uvMemory);
        }

        void Mesh::SetPositions(vk::PhysicalDevice gpu, vk::Device device, std::vector<glm::vec3> data)
        {
            CreateBuffer(gpu, device, data.data(), data.size() * sizeof(glm::vec3), vk::BufferUsageFlagBits::eVertexBuffer, positions, positionsMemory);
        }

        void Mesh::SetNormals(vk::PhysicalDevice gpu, vk::Device device, std::vector<glm::vec3> data)
        {
            CreateBuffer(gpu, device, data.data(), data.size() * sizeof(glm::vec3), vk::BufferUsageFlagBits::eVertexBuffer, normals, normalsMemory);
        }

        void Mesh::SetUV(vk::PhysicalDevice gpu, vk::Device device, std::vector<glm::vec2> data)
        {
            CreateBuffer(gpu, device, data.data(), data.size() * sizeof(glm::vec2), vk::BufferUsageFlagBits::eVertexBuffer, uv, uvMemory);
        }

        void Mesh::SetIndices(vk::PhysicalDevice gpu, vk::Device device, std::vector<uint16_t> data)
        {
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
