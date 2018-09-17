#include "RenderingOptions.hpp"
#include "Mesh.hpp"
#include "Time.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace atlas
{
    namespace graphics
    {
        //Mesh::Mesh(MeshObject mesh, Material material) :
        //    _mesh(mesh),
        //    _material(material),
        //    Drawable()
        //{
        //    _flags |= (int)NodeFlags::Drawable;
        //}

        Mesh::Mesh(uint32_t vertexCount) :
            vertexCount(vertexCount)
        {
            _vertices.resize(vertexCount);
            _flags |= 1 << (uint32_t)NodeFlags::Drawable;
        }

        Mesh::~Mesh()
        {
            // TODO
        }

        void Mesh::Draw(DrawContext context)
        {
            //_localTransform = glm::rotate(_localTransform, static_cast<float>(Time::dt * 0.4), glm::vec3(1, 0, 0));

            //if (_currentPolygonMode != RenderingOptions::PolygonMode)
            //{
            //    DestroyPipeline();
            //    CreatePipeline(_vertexShader.module, _fragmentShader.module);
            //}
            
            struct MVP mvp;
            mvp.model = _transform;
            mvp.view = context.viewMatrix;
            mvp.proj = context.projectionMatrix;

            vk::CommandBuffer cmdBuffer = context.cmdBuffer;
            cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, _material.pipeline);
            cmdBuffer.pushConstants(_material.pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(MVP), &mvp);
            cmdBuffer.setLineWidth(_material.lineWidth);
            //buffer.pushConstants(_material.pipelineLayout, vk::ShaderStageFlagBits::eVertex, sizeof(MVP), sizeof(glm::vec3), &_color);
            //buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, _material.pipelineLayout, 0, 1, _descriptorSets.data(), 0, nullptr);
            vk::DeviceSize offsets[] = { 0 };
            cmdBuffer.bindVertexBuffers(0, 1, &buffer, offsets);

            if (topology == vk::PrimitiveTopology::eTriangleList)
            {
                cmdBuffer.bindIndexBuffer(indices, 0, vk::IndexType::eUint16);
                cmdBuffer.drawIndexed(indexCount, 1, 0, 0, 0);
            }
            else
            {
                cmdBuffer.draw(vertexCount, 1, 0, 0);
            }
        }

        void Mesh::SendSignal(Signal signal)
        {
            if (signal == Signal::WindowResized)
            {
                //TODO
            }
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
                _vertices[i].uv = glm::packHalf2x16(data[i]);
            }
        }

        void Mesh::Apply()
        {
            CreateBuffer(_vertices.data(), _vertices.size() * sizeof(Vertex), vk::BufferUsageFlagBits::eVertexBuffer, buffer, bufferMemory);
        }

        Mesh Mesh::MakePoint(vec3 color, vec3 position)
        {
            Mesh point(1);
            point._flags |= 1 << (uint32_t)NodeFlags::Debug;

            point.topology = vk::PrimitiveTopology::ePointList;
            std::vector<glm::vec3> positions{ position };
            point.SetPositions(positions);

            std::vector<glm::vec3> colors{ color };
            point.SetNormals(colors);
            point.Apply();

            point._material = Material(
                std::vector<Semantic>{Semantic::Position, Semantic::Color},
                std::vector<Descriptor>(),
                Shader::Get("unlit.vert"),
                Shader::Get("unlit.frag"),
                point.topology);

            point._material.lineWidth = 3;

            return point;
        }

        Mesh Mesh::MakeLine(vec3 color, vec3 start, vec3 end)
        {
            Mesh line(2);
            line._flags |= 1 << (uint32_t)NodeFlags::Debug;

            line.topology = vk::PrimitiveTopology::eLineList;
            std::vector<glm::vec3> positions{ start, end };
            line.SetPositions(positions);

            std::vector<glm::vec3> colors{ color, color };
            line.SetNormals(colors);
            line.Apply();

            line._material = Material(
                std::vector<Semantic>{Semantic::Position, Semantic::Color},
                std::vector<Descriptor>(),
                Shader::Get("unlit.vert"),
                Shader::Get("unlit.frag"),
                line.topology);

            return line;
        }

        Mesh Mesh::MakePlane(vec3 color)
        {
            Mesh plane(11*2*2);
            plane._flags |= 1 << (uint32_t)NodeFlags::Debug;

            plane.topology = vk::PrimitiveTopology::eLineList;
            
            std::vector<glm::vec3> positions;
            std::vector<glm::vec3> colors;

            for (size_t i = 0; i <= 10; i++)
            {
                float y = (i / 5.0f) - 1;
                vec3 start = vec3(-1, y, 0);
                vec3 end = vec3( 1, y, 0);
                positions.push_back(start);
                positions.push_back(end);
                colors.push_back(color);
                colors.push_back(color);
            }
            for (size_t i = 0; i <= 10; i++)
            {
                float x = (i / 5.0f) - 1;
                vec3 start = vec3(x, -1, 0);
                vec3 end = vec3(x, 1, 0);
                positions.push_back(start);
                positions.push_back(end);
                colors.push_back(color);
                colors.push_back(color);
            }

            plane.SetPositions(positions);

            plane.SetNormals(colors);
            plane.Apply();

            plane._material = Material(
                std::vector<Semantic>{Semantic::Position, Semantic::Color},
                std::vector<Descriptor>(),
                Shader::Get("unlit.vert"),
                Shader::Get("unlit.frag"),
                plane.topology);

            return plane;
        }

        Mesh Mesh::MakeEllipsoid(vec3 color, double semimajorAxis, double semiminorAxis)
        {
            const int subdivs = 64;
            Mesh ellipsoid(subdivs+1);
            ellipsoid._flags |= 1 << (uint32_t)NodeFlags::Debug;

            ellipsoid.topology = vk::PrimitiveTopology::eLineStrip;

            std::vector<glm::vec3> positions;
            std::vector<glm::vec3> colors;

            float x = 1;
            float y = 0;

            const double circle = PI * 2;
            const double delta = circle / subdivs;
            double angle = 0;

            // equator
            for (size_t i = 0; i <= subdivs; i++)
            {
                x = static_cast<float>(sin(angle));
                y = static_cast<float>(cos(angle));
                angle += delta;
                positions.push_back(vec3(x, y, 0));
                colors.push_back(color);
            }

            ellipsoid.SetPositions(positions);

            ellipsoid.SetNormals(colors);
            ellipsoid.Apply();

            ellipsoid._material = Material(
                std::vector<Semantic>{Semantic::Position, Semantic::Color},
                std::vector<Descriptor>(),
                Shader::Get("unlit.vert"),
                Shader::Get("unlit.frag"),
                ellipsoid.topology);

            return ellipsoid;
        }

        void Mesh::SetIndices(std::vector<uint16_t>& data)
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

        void Mesh::CreateBuffer(void* data, size_t size, vk::BufferUsageFlags usage,
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
