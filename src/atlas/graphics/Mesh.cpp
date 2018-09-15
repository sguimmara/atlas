#include "RenderingOptions.hpp"
#include "Mesh.hpp"
#include "Time.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace atlas
{
    namespace graphics
    {
        Mesh::Mesh(MeshObject mesh, Material material) :
            _mesh(mesh),
            _material(material),
            Drawable()
        {
            _flags |= (int)NodeFlags::Drawable;
        }

        Mesh::~Mesh()
        {
            _mesh.Destroy();
        }

        void Mesh::Draw(DrawContext context)
        {
            _localTransform = glm::rotate(_localTransform, static_cast<float>(Time::dt * 0.4), glm::vec3(0, 1, 0));

            //if (_currentPolygonMode != RenderingOptions::PolygonMode)
            //{
            //    DestroyPipeline();
            //    CreatePipeline(_vertexShader.module, _fragmentShader.module);
            //}
            
            struct MVP mvp;
            mvp.model = _transform;
            mvp.view = context.viewMatrix;
            mvp.proj = context.projectionMatrix;

            vk::CommandBuffer buffer = context.cmdBuffer;
            buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, _material.pipeline);
            buffer.pushConstants(_material.pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(MVP), &mvp);
            //buffer.pushConstants(_material.pipelineLayout, vk::ShaderStageFlagBits::eVertex, sizeof(MVP), sizeof(glm::vec3), &_color);
            //buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, _material.pipelineLayout, 0, 1, _descriptorSets.data(), 0, nullptr);
            buffer.bindVertexBuffers(0, { _mesh.buffer }, { 0 });

            if (_mesh.topology == vk::PrimitiveTopology::eTriangleList)
            {
                buffer.bindIndexBuffer(_mesh.indices, 0, vk::IndexType::eUint16);
                buffer.drawIndexed(_mesh.indexCount, 1, 0, 0, 0);
            }
            else
            {
                buffer.draw(_mesh.vertexCount, 1, 0, 0);
            }
        }

        void Mesh::SendSignal(Signal signal)
        {
            if (signal == Signal::WindowResized)
            {
                //TODO
            }
        }
    }
}
