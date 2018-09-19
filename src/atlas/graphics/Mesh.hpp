#ifndef ATLAS_GRAPHICS_MESH_HPP
#define ATLAS_GRAPHICS_MESH_HPP

#include "AtlasGraphics.hpp"
#include "Node.hpp"
#include "Drawable.hpp"
#include "Renderer.hpp"
#include "Shader.hpp"
#include "Vertex.hpp"
#include "Material.hpp"
#include "Image.hpp"

#include "atlas/core/Ellipsoid.hpp"

using namespace atlas::core;

namespace atlas
{
    namespace graphics
    {
        /**
        * @brief A Mesh is a Drawable with vertices organized in a mesh
        */
        class Mesh : public Drawable
        {
        public:
            Mesh(uint32_t vertexCount);
            ~Mesh();

            /**
            * @brief Fills the command buffer with draw commands
            */
            void Draw(DrawContext context);

            void SendSignal(Signal signal);

            uint32_t indexCount;
            uint32_t vertexCount;
            vk::Buffer indices;
            vk::IndexType indexType;
            vk::PrimitiveTopology topology;

            void SetIndices(std::vector<uint16_t>& data);
            void SetPositions(std::vector<vec3>& data);
            void SetNormals(std::vector<vec3>& data);
            void SetUV(std::vector<vec2>& data);
            void Apply();

            static Mesh MakePoint(vec3 color, vec3 position);
            static Mesh MakeLine(vec3 color, vec3 start, vec3 end);
            static Mesh MakePlane(vec3 color);
            static Mesh MakeParallel(vec3 color, double lat, Ellipsoid& ellipsoid);
            static Mesh MakeMeridian(vec3 color, double lon, Ellipsoid& ellipsoid);
            static Mesh MakeEllipsoid(vec3 color, uint32_t subdivs, Ellipsoid& ellipsoid);
            static Mesh MakeSolidEllipsoid(vec3 color, uint32_t subdivs, Ellipsoid& ellipsoid);
            static Mesh MakeRegion(vec3 color, vec2 min, vec2 max, Ellipsoid& ellipsoid);
            static Mesh MakeFrustum(vec3 color, mat4 direction, vec3 origin, float aspect, float fovRadians, float nearClip, float farClip);

        private:
            vk::DeviceMemory indicesMemory;
            vk::DeviceMemory bufferMemory;
            std::vector<Vertex> _vertices;

            static void CreateBuffer(void* data, size_t size, vk::BufferUsageFlags usage, vk::Buffer& buf, vk::DeviceMemory& memory);

        protected:
            vk::Buffer buffer;
            Material _material;
            vec3 _color;
        };
    }
}

#endif