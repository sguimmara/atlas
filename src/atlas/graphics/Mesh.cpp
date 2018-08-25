#include "Mesh.hpp"

#include "VulkanTools.hpp"
#include "VulkanInitializers.hpp"

using namespace vks::initializers;

namespace atlas
{
    namespace graphics
    {
        Mesh::Mesh(Renderer* renderer, Shader* vertexShader, Shader* fragmentShader) :
            renderer(renderer),
            pool(renderer->commandPool()),
            device(renderer->device()),
            fragmentShader(fragmentShader),
            vertexShader(vertexShader),
            Drawable()
        {
            createDescriptorPool();
            //createDescriptorSetLayout();
            //createDescriptorSets();
        }

        Mesh::~Mesh()
        {
            destroyBuffer(vertexBuffer, vertexBufferMemory);
            destroyBuffer(colorBuffer, colorBufferMemory);
            destroyCommandBuffer();
            destroyDescriptorSetLayout();
            destroyGraphicsPipeline();
            destroyDescriptorSets();
            destroyDescriptorPool();
        }

        void Mesh::createDescriptorPool()
        {
            VkDescriptorPoolSize poolSize = {};
            poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            poolSize.descriptorCount = 1;

            VkDescriptorPoolCreateInfo poolInfo = {};
            poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            poolInfo.poolSizeCount = 1;
            poolInfo.pPoolSizes = &poolSize;
            poolInfo.maxSets = 1;

            VK_CHECK_RESULT(vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool));
        }

        void Mesh::destroyDescriptorPool()
        {
            if (descriptorPool != VK_NULL_HANDLE)
            {
                vkDestroyDescriptorPool(device, descriptorPool, nullptr);
                descriptorPool = VK_NULL_HANDLE;
            }
        }

        void Mesh::createDescriptorSets()
        {
            VkDescriptorSetAllocateInfo allocInfo = {};
            allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocInfo.descriptorPool = descriptorPool;
            allocInfo.descriptorSetCount = 1;
            allocInfo.pSetLayouts = &descriptorSetLayout;

            VK_CHECK_RESULT(vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet));
        }

        void Mesh::destroyDescriptorSets()
        {
            //vkFreeDescriptorSets(device, descriptorPool, 1, &descriptorSet);
        }

        VkCommandBuffer Mesh::getCommandBuffer(uint32_t imageIndex)
        {
            return commandBuffers[imageIndex];
        }

        void Mesh::setBuffer(void * data, size_t count, size_t elementSize, VkBuffer& target, VkDeviceMemory& mem, VkBufferUsageFlags flags)
        {
            createBuffer(target, data, count * elementSize, mem, flags);
        }

        void Mesh::setPositions(std::vector<glm::vec3> positions)
        {
            setBuffer(positions.data(), positions.size(), sizeof(glm::vec3), vertexBuffer, vertexBufferMemory, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        }

        void Mesh::setColors(std::vector<glm::vec3> colors)
        {
            setBuffer(colors.data(), colors.size(), sizeof(glm::vec3), colorBuffer, colorBufferMemory, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        }

        void Mesh::updateDescriptorSets()
        {
            VkWriteDescriptorSet descriptorWrite = {};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = descriptorSet;
            descriptorWrite.dstBinding = 0;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrite.descriptorCount = 0;
            descriptorWrite.pBufferInfo = nullptr;

            vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
        }

        void Mesh::prepareRenderState()
        {
            destroyCommandBuffer();
            destroyGraphicsPipeline();

            //updateDescriptorSets();
            createGraphicsPipeline();
            createCommandBuffer();
        }

        void Mesh::tick(float dt)
        {
            //updateCommandBuffers();

            //setModelViewProjectionMatrix(mvp);
        }

        void Mesh::createBuffer(VkBuffer& buf, void* bufferData, size_t size, VkDeviceMemory& memory, VkBufferUsageFlags flags)
        {
            destroyBuffer(buf, memory);

            VkBufferCreateInfo bufferInfo = vks::initializers::bufferCreateInfo();
            bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferInfo.size = size;
            bufferInfo.usage = flags;
            bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            VK_CHECK_RESULT(vkCreateBuffer(device, &bufferInfo, nullptr, &buf));

            VkMemoryRequirements memreq;
            vkGetBufferMemoryRequirements(device, buf, &memreq);

            VkMemoryAllocateInfo allocInfo = {};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memreq.size;
            allocInfo.memoryTypeIndex = renderer->findMemoryType(memreq.memoryTypeBits,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

            void* data;
            VK_CHECK_RESULT(vkAllocateMemory(device, &allocInfo, nullptr, &memory));
            VK_CHECK_RESULT(vkBindBufferMemory(device, buf, memory, 0));
            VK_CHECK_RESULT(vkMapMemory(device, memory, 0, bufferInfo.size, 0, &data));
            memcpy(data, bufferData, (size_t)bufferInfo.size);
            vkUnmapMemory(device, memory);
        }

        void Mesh::destroyBuffer(VkBuffer& buffer, VkDeviceMemory& memory)
        {
            if (buffer != VK_NULL_HANDLE)
            {
                vkDestroyBuffer(device, buffer, nullptr);
                buffer = VK_NULL_HANDLE;
            }
            if (memory != VK_NULL_HANDLE)
            {
                vkFreeMemory(device, memory, nullptr);
                memory = VK_NULL_HANDLE;
            }
        }

        void Mesh::createGraphicsPipeline()
        {
            std::vector<VkVertexInputBindingDescription> inputBindings;
            std::vector<VkVertexInputAttributeDescription> inputAttribs;

            // positions
            VkVertexInputBindingDescription posDesc = {};
            posDesc.binding = 0;
            posDesc.stride = sizeof(glm::vec3);
            posDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            inputBindings.push_back(posDesc);

            VkVertexInputAttributeDescription posAttrib = {};
            posAttrib.binding = posDesc.binding;
            posAttrib.location = 0;
            posAttrib.format = VK_FORMAT_R32G32B32_SFLOAT;
            posAttrib.offset = 0;

            inputAttribs.push_back(posAttrib);

            // colors
            VkVertexInputBindingDescription colorDesc = {};
            colorDesc.binding = 1;
            colorDesc.stride = sizeof(glm::vec3);
            colorDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            inputBindings.push_back(colorDesc);

            VkVertexInputAttributeDescription colAttrib = {};
            colAttrib.binding = colorDesc.binding;
            colAttrib.location = 1;
            colAttrib.format = VK_FORMAT_R32G32B32_SFLOAT;
            colAttrib.offset = 0;

            inputAttribs.push_back(colAttrib);

            VkViewport viewport = {};
            viewport.x = 0;
            viewport.y = 0;
            viewport.width = static_cast<float>(renderer->framebuffer()->getExtent().width);
            viewport.height = static_cast<float>(renderer->framebuffer()->getExtent().height);

            VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
            vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
            vertShaderStageInfo.module = vertexShader->shaderModule();
            vertShaderStageInfo.pName = "main";

            VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
            fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            fragShaderStageInfo.module = fragmentShader->shaderModule();
            fragShaderStageInfo.pName = "main";

            VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

            VkPipelineVertexInputStateCreateInfo vertexInputInfo = pipelineVertexInputStateCreateInfo();
            vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(inputBindings.size());
            vertexInputInfo.pVertexBindingDescriptions = inputBindings.data();
            vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(inputAttribs.size());
            vertexInputInfo.pVertexAttributeDescriptions = inputAttribs.data();

            VkPipelineInputAssemblyStateCreateInfo inputAssembly = pipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);

            VkRect2D scissor = {};
            scissor.offset = { 0, 0 };
            scissor.extent = VkExtent2D{ (uint32_t)viewport.width, (uint32_t)viewport.height };

            VkPipelineViewportStateCreateInfo viewportState = pipelineViewportStateCreateInfo(1, 1);
            viewportState.pViewports = &viewport;
            viewportState.pScissors = &scissor;

            VkPipelineRasterizationStateCreateInfo rasterizer = pipelineRasterizationStateCreateInfo(
                VK_POLYGON_MODE_FILL,
                VK_CULL_MODE_NONE,
                VK_FRONT_FACE_CLOCKWISE);

            VkPipelineMultisampleStateCreateInfo multisampling = pipelineMultisampleStateCreateInfo(
                VK_SAMPLE_COUNT_1_BIT);

            VkPipelineColorBlendAttachmentState colorBlendAttachment = pipelineColorBlendAttachmentState(
                VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
                VK_FALSE);

            VkPipelineColorBlendStateCreateInfo colorBlending = pipelineColorBlendStateCreateInfo(1, &colorBlendAttachment);

            std::vector<VkDynamicState> dynamicStates = {
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_LINE_WIDTH
            };

            VkPushConstantRange mvpConstant = {};
            mvpConstant.offset = 0;
            mvpConstant.size = sizeof(glm::mat4);
            mvpConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

            VkPipelineDynamicStateCreateInfo dynamicState = pipelineDynamicStateCreateInfo(dynamicStates);
            VkPipelineLayoutCreateInfo pipelineLayoutInfo = pipelineLayoutCreateInfo(nullptr, 0);
            pipelineLayoutInfo.pushConstantRangeCount = 1;
            pipelineLayoutInfo.pPushConstantRanges = &mvpConstant;

            VK_CHECK_RESULT(vkCreatePipelineLayout(this->device, &pipelineLayoutInfo, nullptr, &pipelineLayout));

            VkGraphicsPipelineCreateInfo pipelineInfo = pipelineCreateInfo(pipelineLayout, renderer->framebuffer()->getRenderPass());
            pipelineInfo.stageCount = 2;
            pipelineInfo.pStages = shaderStages;
            pipelineInfo.pVertexInputState = &vertexInputInfo;
            pipelineInfo.pInputAssemblyState = &inputAssembly;
            pipelineInfo.pViewportState = &viewportState;
            pipelineInfo.pRasterizationState = &rasterizer;
            pipelineInfo.pMultisampleState = &multisampling;
            pipelineInfo.pColorBlendState = &colorBlending;
            pipelineInfo.subpass = 0;

            VK_CHECK_RESULT(vkCreateGraphicsPipelines(this->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline));
        }

        void Mesh::destroyGraphicsPipeline()
        {
            if (pipeline != VK_NULL_HANDLE)
            {
                vkDestroyPipeline(device, pipeline, nullptr);
                pipeline = VK_NULL_HANDLE;
            }
            if (pipelineLayout != VK_NULL_HANDLE)
            {
                vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
                pipelineLayout = VK_NULL_HANDLE;
            }
        }

        void Mesh::createCommandBuffer()
        {
            destroyCommandBuffer();

            commandBuffers.resize(renderer->framebuffer()->size());
            assert(commandBuffers.size() > 0);

            VkCommandBufferAllocateInfo allocInfo = commandBufferAllocateInfo(pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, (uint32_t)commandBuffers.size());
            VK_CHECK_RESULT(vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()));

            updateCommandBuffers();
        }

        void Mesh::updateCommandBuffers()
        {
            for (size_t i = 0; i < commandBuffers.size(); i++)
            {
                VkCommandBufferBeginInfo beginInfo = commandBufferBeginInfo();
                beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

                VK_CHECK_RESULT(vkBeginCommandBuffer(commandBuffers[i], &beginInfo));
                {
                    VkExtent2D extent = renderer->framebuffer()->getExtent();

                    VkRenderPassBeginInfo renderPassInfo = renderPassBeginInfo();
                    renderPassInfo.renderPass = renderer->framebuffer()->getRenderPass();
                    renderPassInfo.framebuffer = renderer->framebuffer()->getFramebuffer(i);
                    renderPassInfo.renderArea.offset = { 0, 0 };
                    renderPassInfo.renderArea.extent = renderer->framebuffer()->getExtent();

                    renderPassInfo.clearValueCount = 1;
                    renderPassInfo.pClearValues = renderer->clearColor();

                    vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
                    {
                        vkCmdPushConstants(commandBuffers[i], pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &mModelView);
                        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
                        //vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

                        VkBuffer buffers[] = { vertexBuffer, colorBuffer };
                        VkDeviceSize offsets[] = { 0, 0 };
                        vkCmdBindVertexBuffers(commandBuffers[i], 0, 2, buffers, offsets);

                        vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);
                    }
                    vkCmdEndRenderPass(commandBuffers[i]);
                }
                VK_CHECK_RESULT(vkEndCommandBuffer(commandBuffers[i]));
            }
        }

        void Mesh::destroyCommandBuffer()
        {
            if (commandBuffers.size() > 0)
            {
                vkFreeCommandBuffers(device, pool, (uint32_t)commandBuffers.size(), commandBuffers.data());
                commandBuffers.resize(0);
            }
        }

        void Mesh::createDescriptorSetLayout()
        {
            VkDescriptorSetLayoutBinding binding = {};
            binding.binding = 0;
            binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            binding.descriptorCount = 1;
            binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

            VkDescriptorSetLayoutCreateInfo layoutInfo = {};
            layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layoutInfo.bindingCount = 1;
            layoutInfo.pBindings = &binding;

            VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout));
        }

        void Mesh::destroyDescriptorSetLayout()
        {
            if (descriptorSetLayout != VK_NULL_HANDLE)
            {
                vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
            }
        }
    }
}