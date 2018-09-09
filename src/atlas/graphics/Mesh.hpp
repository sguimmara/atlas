#ifndef ATLAS_GRAPHICS_MESH_HPP
#define ATLAS_GRAPHICS_MESH_HPP

#include "AtlasGraphics.hpp"

namespace atlas
{
    namespace graphics
    {
        struct Vertex
        {
            glm::vec3 position;
            glm::vec3 normal;
            uint32_t uv;
        };

        struct Mesh
        {
            uint32_t indexCount;
            vk::Buffer indices;
            vk::IndexType indexType;

            vk::Buffer buffer;

            Mesh(uint32_t vertexCount);

            void Destroy(vk::Device device);
            void SetIndices(vk::PhysicalDevice gpu, vk::Device device, std::vector<uint16_t>& data);
            void SetPositions(std::vector<glm::vec3>& data);
            void SetNormals(std::vector<glm::vec3>& data);
            void SetUV(std::vector<glm::vec2>& data);
            void Apply(vk::PhysicalDevice gpu, vk::Device device);

        private:
            vk::DeviceMemory indicesMemory;
            vk::DeviceMemory bufferMemory;
            std::vector<Vertex> _vertices;

            static void CreateBuffer(vk::PhysicalDevice gpu, vk::Device device,
                void* data, size_t size, vk::BufferUsageFlags usage,
                vk::Buffer& buf, vk::DeviceMemory& memory);
        };
    }
}

#endif
