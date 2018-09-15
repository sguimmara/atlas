#ifndef ATLAS_GRAPHICS_MESHOBJECT_HPP
#define ATLAS_GRAPHICS_MESHOBJECT_HPP

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

        struct MeshObject
        {
            uint32_t indexCount;
            uint32_t vertexCount;
            vk::Buffer indices;
            vk::IndexType indexType;
            vk::PrimitiveTopology topology;

            vk::Buffer buffer;

            MeshObject(uint32_t vertexCount);

            void Destroy();
            void SetIndices(std::vector<uint16_t>& data);
            void SetPositions(std::vector<glm::vec3>& data);
            void SetNormals(std::vector<glm::vec3>& data);
            void SetUV(std::vector<glm::vec2>& data);
            void Apply();

        private:
            vk::DeviceMemory indicesMemory;
            vk::DeviceMemory bufferMemory;
            std::vector<Vertex> _vertices;

            static void CreateBuffer(void* data, size_t size, vk::BufferUsageFlags usage, vk::Buffer& buf, vk::DeviceMemory& memory);
        };
    }
}

#endif
