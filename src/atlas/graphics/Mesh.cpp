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
            Mesh plane(11 * 2 * 2);
            plane._flags |= 1 << (uint32_t)NodeFlags::Debug;

            plane.topology = vk::PrimitiveTopology::eLineList;

            std::vector<glm::vec3> positions;
            std::vector<glm::vec3> colors;

            for (size_t i = 0; i <= 10; i++)
            {
                float y = (i / 5.0f) - 1;
                vec3 start = vec3(-1, y, 0);
                vec3 end = vec3(1, y, 0);
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

        vec3 latLonToECEF(double lat, double lon, Ellipsoid& ellipsoid)
        {
            const double scale = 0.0000005;

            const double a = ellipsoid.semimajorAxis() * scale;
            const double b = ellipsoid.semiminorAxis() * scale;
            const double e2 = 1 - (b * b) / (a * a);

            double sinlat = std::sin(lat);
            double sinlon = std::sin(lon);
            double coslon = std::cos(lon);
            double coslat = std::cos(lat);

            double nLat = a / std::sqrt(1 - e2 * sinlat);

            double x = (nLat)* coslat * coslon;
            double z = (nLat)* coslat * sinlon;
            double y = (((b*b) / (a*a)*nLat)) * sinlat;

            return vec3{ x, z, y };
        }

        Mesh Mesh::MakeParallel(vec3 color, double lat, Ellipsoid& ellipsoid)
        {
            std::vector<glm::vec3> positions;
            std::vector<glm::vec3> colors;

            double lonDelta = PI * 2 / 64;

            for (size_t i = 0; i <= 64; i++)
            {
                double lon = (i * lonDelta) - PI;
                auto xyz = latLonToECEF(lat, lon, ellipsoid);
                positions.push_back(xyz);
                colors.push_back(color);
            }

            Mesh ellipse(static_cast<uint32_t>(positions.size()));
            ellipse._flags |= 1 << (uint32_t)NodeFlags::Debug;
            ellipse.topology = vk::PrimitiveTopology::eLineStrip;
            ellipse.SetPositions(positions);

            ellipse.SetNormals(colors);
            ellipse.Apply();

            ellipse._material = Material(
                std::vector<Semantic>{Semantic::Position, Semantic::Color},
                std::vector<Descriptor>(),
                Shader::Get("unlit.vert"),
                Shader::Get("unlit.frag"),
                ellipse.topology);
            ellipse._material.lineWidth = 2;

            return ellipse;
        }

        Mesh Mesh::MakeMeridian(vec3 color, double lon, Ellipsoid& ellipsoid)
        {
            std::vector<glm::vec3> positions;
            std::vector<glm::vec3> colors;

            double latDelta = PI / 64;

            for (size_t i = 0; i <= 64; i++)
            {
                double lat = (i * latDelta) - PI / 2;
                auto xyz = latLonToECEF(lat, lon, ellipsoid);
                positions.push_back(xyz);
                colors.push_back(color);
            }

            Mesh ellipse(static_cast<uint32_t>(positions.size()));
            ellipse._flags |= 1 << (uint32_t)NodeFlags::Debug;
            ellipse.topology = vk::PrimitiveTopology::eLineStrip;
            ellipse.SetPositions(positions);

            ellipse.SetNormals(colors);
            ellipse.Apply();

            ellipse._material = Material(
                std::vector<Semantic>{Semantic::Position, Semantic::Color},
                std::vector<Descriptor>(),
                Shader::Get("unlit.vert"),
                Shader::Get("unlit.frag"),
                ellipse.topology);
            ellipse._material.lineWidth = 2;

            return ellipse;
        }

        Mesh Mesh::MakeEllipsoid(vec3 color, uint32_t subdivs, Ellipsoid& ellipsoid)
        {
            std::vector<glm::vec3> positions;
            std::vector<glm::vec3> colors;

            double lonDelta, latDelta;
            double lat, lon;

            lonDelta = PI * 2 / subdivs;
            latDelta = PI / 64;

            // meridians
            for (size_t i = 0; i < subdivs; i += 2)
            {
                lon = (i * lonDelta) - PI;

                // meridians are drawn by pairs (m0, m1)
                // m0 is drawn from the south pole to the north pole.
                // m1 is drawn from the north pole to the south pole.
                // This scheme enables using a single line strip to draw all
                // meridians without visual artifacts.

                for (int j = 0; j <= 64; j++)
                {
                    lat = (j * latDelta) - PI / 2;
                    auto xyz = latLonToECEF(lat, lon, ellipsoid);
                    positions.push_back(xyz);
                    colors.push_back(color);
                }

                lon = ((i + 1) * lonDelta) - PI;
                for (int j = 64; j >= 0; j--)
                {
                    lat = (j * latDelta) - PI / 2;
                    auto xyz = latLonToECEF(lat, lon, ellipsoid);
                    positions.push_back(xyz);
                    colors.push_back(color);
                }
            }

            lonDelta = PI * 2 / 64;
            latDelta = PI / subdivs;

            // parallels
            for (size_t j = 0; j < subdivs; j++)
            {
                lat = (j * latDelta) - PI / 2;
                for (size_t i = 0; i <= 64; i++)
                {
                    lon = (i * lonDelta) - PI;
                    auto xyz = latLonToECEF(lat, lon, ellipsoid);
                    positions.push_back(xyz);
                    colors.push_back(color);
                }
            }



            Mesh ellipse(static_cast<uint32_t>(positions.size()));
            ellipse._flags |= 1 << (uint32_t)NodeFlags::Debug;
            ellipse.topology = vk::PrimitiveTopology::eLineStrip;
            ellipse.SetPositions(positions);

            ellipse.SetNormals(colors);
            ellipse.Apply();

            ellipse._material = Material(
                std::vector<Semantic>{Semantic::Position, Semantic::Color},
                std::vector<Descriptor>(),
                Shader::Get("unlit.vert"),
                Shader::Get("unlit.frag"),
                ellipse.topology);

            return ellipse;
        }

        Mesh Mesh::MakeSolidEllipsoid(vec3 color, uint32_t subdivs, Ellipsoid& ellipsoid)
        {
            const double minX = 0;
            const double maxX = PI * 2;
            const double minY = -PI / 2;
            const double maxY = PI / 2;

            const uint16_t subdivX = subdivs * 2;
            const uint16_t subdivY = subdivs;

            const double xStep = (maxX - minX) / subdivX;
            const double yStep = (maxY - minY) / subdivY;

            const uint16_t vertexPerRow = subdivY + 1;
            const uint16_t vertexPerCol = subdivX + 1;
            const int size = vertexPerRow * vertexPerCol;

            std::vector<glm::vec3> positions;
            std::vector<glm::vec3> colors;
            std::vector<uint16_t> indices;

            positions.resize(size);
            colors.resize(size);

            int i = 0;

            for (uint16_t row = 0; row < vertexPerRow; ++row)
            {
                uint16_t jww = row * vertexPerCol;
                uint16_t j1ww = (row + 1) * vertexPerCol;

                for (uint16_t col = 0; col < vertexPerCol; ++col)
                {
                    double lat = minY + row * yStep;
                    double lon = minX + col * xStep;

                    positions[i] = latLonToECEF(lat, lon, ellipsoid);

                    colors[i] = color;

                    ++i;

                    if (row < subdivY && col < subdivX)
                    {
                        uint16_t a = col + jww;
                        uint16_t b = (col + 1) + jww;
                        uint16_t c = (col + 1) + j1ww;
                        uint16_t d = col + j1ww;

                        indices.push_back(a);
                        indices.push_back(b);
                        indices.push_back(c);

                        indices.push_back(c);
                        indices.push_back(d);
                        indices.push_back(a);
                    }
                }
            }

            Mesh result(static_cast<uint32_t>(positions.size()));
            result.SetPositions(positions);
            result.SetNormals(colors);
            result.SetIndices(indices);
            result.topology = vk::PrimitiveTopology::eTriangleList;
            result.Apply();
            result._flags |= 1 << (uint32_t)NodeFlags::Debug;

            result._material = Material(
                std::vector<Semantic>{Semantic::Position, Semantic::Color},
                std::vector<Descriptor>(),
                Shader::Get("unlit.vert"),
                Shader::Get("unlit.frag"),
                result.topology);

            return result;
        }

        Mesh Mesh::MakeRegion(vec3 color, vec2 min, vec2 max, Ellipsoid& ellipsoid)
        {
            const double minX = min.x;
            const double maxX = max.x;
            const double minY = min.y;
            const double maxY = max.y;

            std::vector<glm::vec3> positions;
            std::vector<glm::vec3> colors;

            uint32_t subdivs = 10;

            double w = std::abs(max.x - min.x);
            double h = std::abs(max.y - min.y);

            // top edge
            for (uint32_t i = 0; i < subdivs; i++)
            {
                double lat = max.y;
                double lon = min.x + (w / subdivs) * i;

                vec3 pos = latLonToECEF(lat, lon, ellipsoid);
                positions.push_back(pos);
                colors.push_back(color);
            }

            // right edge
            for (uint32_t i = 0; i < subdivs; i++)
            {
                double lon = max.x;
                double lat = max.y - (h / subdivs) * i;

                vec3 pos = latLonToECEF(lat, lon, ellipsoid);
                positions.push_back(pos);
                colors.push_back(color);
            }

            // bottom edge
            for (uint32_t i = 0; i < subdivs; i++)
            {
                double lon = max.x - (w / subdivs) * i;
                double lat = min.y;

                vec3 pos = latLonToECEF(lat, lon, ellipsoid);
                positions.push_back(pos);
                colors.push_back(color);
            }

            // left edge
            for (uint32_t i = 0; i <= subdivs; i++)
            {
                double lon = min.x;
                double lat = min.y + (h / subdivs) * i;

                vec3 pos = latLonToECEF(lat, lon, ellipsoid);
                positions.push_back(pos);
                colors.push_back(color);
            }

            Mesh result(static_cast<uint32_t>(positions.size()));
            result.SetPositions(positions);
            result.SetNormals(colors);
            result.topology = vk::PrimitiveTopology::eLineStrip;
            result.Apply();
            result._flags |= 1 << (uint32_t)NodeFlags::Debug;

            result._material = Material(
                std::vector<Semantic>{Semantic::Position, Semantic::Color},
                std::vector<Descriptor>(),
                Shader::Get("unlit.vert"),
                Shader::Get("unlit.frag"),
                result.topology);
            result._material.lineWidth = 2;

            return result;
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
