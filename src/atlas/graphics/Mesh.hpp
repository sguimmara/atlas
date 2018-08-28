#ifndef ATLAS_GRAPHICS_MESH_HPP
#define ATLAS_GRAPHICS_MESH_HPP

#include "AtlasGraphics.hpp"

namespace atlas
{
    namespace graphics
    {
        struct Mesh
        {
            uint32_t vertexCount;
            vk::Buffer indices;
            vk::IndexType indexType;

            vk::Buffer positions;
            vk::Buffer normals;
            vk::Buffer uv;

            void Destroy(vk::Device device);

        private:
            vk::DeviceMemory indicesMemory;
            vk::DeviceMemory positionsMemory;
            vk::DeviceMemory normalsMemory;
            vk::DeviceMemory uvMemory;

            void SetIndices(vk::PhysicalDevice gpu, vk::Device device, std::vector<uint16_t> data);
            void SetPositions(vk::PhysicalDevice gpu, vk::Device device, std::vector<glm::vec3> data);
            void SetNormals(vk::PhysicalDevice gpu, vk::Device device, std::vector<glm::vec3> data);
            void SetUV(vk::PhysicalDevice gpu, vk::Device device, std::vector<glm::vec2> data);
            static void CreateBuffer(vk::PhysicalDevice gpu, vk::Device device,
                void* data, size_t size, vk::BufferUsageFlags usage,
                vk::Buffer& buf, vk::DeviceMemory& memory);
        };
    }
}

#endif
