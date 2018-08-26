#include "Blank.hpp"

namespace atlas
{
    namespace graphics
    {
        void Blank::prepareRenderState()
        {
        }

        void Blank::UpdateCommandBuffers()
        {
            //vk::Result result;

            //for (uint32_t i = 0; i < _commandBuffers.size(); i++)
            //{
            //    auto const begin = vk::CommandBufferBeginInfo()
            //        .setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse);

            //    result = _commandBuffers[i].begin(&begin);
            //    VERIFY(result == vk::Result::eSuccess);

            //    auto extent = _renderer->extent();
            //    const auto renderPassBegin = vk::RenderPassBeginInfo()
            //        .setRenderPass(_renderer->framebuffer()->renderPass())
            //        .setFramebuffer(_renderer->framebuffer()->framebuffer(i))
            //        .setRenderArea(vk::Rect2D(vk::Offset2D(0, 0), extent))
            //        .setClearValueCount(1)
            //        .setPClearValues(vk::ClearValue(0, 0, 0, 0));

            //    _commandBuffers[i].beginRenderPass(&renderPassBegin, vk::SubpassContents::eInline);
            //    {
            //        _commandBuffers[i].bindPipeline()
            //    }
            //        
            //}
            //        void Mesh::updateCommandBuffers()
            //        {
            //            for (size_t i = 0; i < commandBuffers.size(); i++)
            //            {
            //                VkCommandBufferBeginInfo beginInfo = commandBufferBeginInfo();
            //                beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
            //
            //                VK_CHECK_RESULT(vkBeginCommandBuffer(commandBuffers[i], &beginInfo));
            //                {
            //                    VkExtent2D extent = renderer->framebuffer()->getExtent();
            //
            //                    // TODO
            //                    VkRenderPassBeginInfo renderPassInfo = renderPassBeginInfo();
            //                    //renderPassInfo.renderPass = renderer->framebuffer()->getRenderPass();
            //                    //renderPassInfo.framebuffer = renderer->framebuffer()->getFramebuffer(i);
            //                    //renderPassInfo.renderArea.offset = { 0, 0 };
            //                    //renderPassInfo.renderArea.extent = renderer->framebuffer()->getExtent();
            //
            //                    //renderPassInfo.clearValueCount = 1;
            //                    //renderPassInfo.pClearValues = renderer->clearColor();
            //
            //                    vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
            //                    {
            //                        vkCmdPushConstants(commandBuffers[i], pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &mModelView);
            //                        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
            //                        //vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
            //
            //                        VkBuffer buffers[] = { vertexBuffer, colorBuffer };
            //                        VkDeviceSize offsets[] = { 0, 0 };
            //                        vkCmdBindVertexBuffers(commandBuffers[i], 0, 2, buffers, offsets);
            //
            //                        vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);
            //                    }
            //                    vkCmdEndRenderPass(commandBuffers[i]);
            //                }
            //                VK_CHECK_RESULT(vkEndCommandBuffer(commandBuffers[i]));
            //            }
            //        }
        }

        vk::CommandBuffer Blank::GetCommandBuffer(uint32_t imageIndex)
        {
            return vk::CommandBuffer();
        }
    }
}

