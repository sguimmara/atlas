#ifndef ATLAS_MESH_HPP
#define ATLAS_MESH_HPP

#include "AtlasGraphics.hpp"

#include "Renderer2.hpp"
#include "Drawable.hpp"
#include "Shader.hpp"

namespace atlas
{
    namespace graphics
    {
        class Mesh : public Drawable
        {
        public:
            Mesh(Renderer2* renderer, Shader* vertexShader, Shader* fragmentShader);
            ~Mesh();

            VkCommandBuffer getCommandBuffer(uint32_t imageIndex);
            void prepareRenderState() override;
            void tick(float dt);

            void setPositions(std::vector<glm::vec3>);
            void setColors(std::vector<glm::vec3>);

        private:
            Renderer2 * renderer = nullptr;
            Shader* vertexShader = nullptr;
            Shader* fragmentShader = nullptr;

            VkDevice device = VK_NULL_HANDLE;
            VkCommandPool pool = VK_NULL_HANDLE;
            VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
            VkPipeline pipeline = VK_NULL_HANDLE;
            std::vector<VkCommandBuffer> commandBuffers;

            VkBuffer vertexBuffer = VK_NULL_HANDLE;
            VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;

            VkBuffer colorBuffer = VK_NULL_HANDLE;
            VkDeviceMemory colorBufferMemory = VK_NULL_HANDLE;

            VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
            VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
            VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

            void createGraphicsPipeline();
            void destroyGraphicsPipeline();
            void createBuffer(VkBuffer&, void*, size_t, VkDeviceMemory&, VkBufferUsageFlags);
            void destroyBuffer(VkBuffer&, VkDeviceMemory&);
            void createCommandBuffer();
            void updateCommandBuffers();
            void destroyCommandBuffer();
            void createDescriptorSetLayout();
            void destroyDescriptorSetLayout();
            void createDescriptorPool();
            void destroyDescriptorPool();
            void createDescriptorSets();
            void destroyDescriptorSets();
            void updateDescriptorSets();

            void setBuffer(void * data, size_t count, size_t elementSize, VkBuffer& target, VkDeviceMemory& mem, VkBufferUsageFlags flags);
        };
    }
}

#endif
