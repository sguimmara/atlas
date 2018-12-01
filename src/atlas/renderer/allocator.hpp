#ifndef ATLAS_RENDERER_ALLOCATOR_HPP
#define ATLAS_RENDERER_ALLOCATOR_HPP

#include <unordered_map>
#include "common.hpp"

#include <vma/vk_mem_alloc.h>

namespace atlas::renderer
{
    class Allocator
    {
    public:
        static void init(vk::Device, vk::PhysicalDevice, uint32_t transferFamily, vk::Queue transfer);
        static void terminate();

        static vk::CommandBuffer beginSingleTimeCommand();
        static void endSingleTimeCommand(vk::CommandBuffer);
        static vk::Buffer getBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage);
        static vk::Buffer getStagingBuffer(vk::DeviceSize);
        static vk::Image getImage(vk::Extent2D extents, vk::Format format,
            vk::ImageUsageFlags usage,
            vk::ImageTiling tiling = vk::ImageTiling::eOptimal);
        static void transition(vk::Image, vk::Format, vk::ImageLayout from, vk::ImageLayout to);
        static void write(vk::Buffer, void* data, size_t size);
        static void free(vk::Buffer);
        static void free(vk::Image);

    private:
        static std::shared_ptr<spdlog::logger> _log;
        static vk::Device _device;
        static vk::CommandPool _commandPool;
        static vk::Queue _transferQueue;
        static vk::PhysicalDevice _physicalDevice;
        static std::unordered_map<vk::Buffer, VmaAllocation> _allocatedBuffers;
        static std::unordered_map<vk::Image, VmaAllocation> _allocatedImages;

        static VmaAllocator _vma;
    };
}

namespace std
{
    template <>
    struct hash<vk::Buffer>
    {
        inline size_t operator()(const vk::Buffer& buf) const
        {
            auto cast = (VkBuffer)(buf);
            return (size_t)cast;
        }
    };

    template <>
    struct hash<vk::Image>
    {
        inline size_t operator()(const vk::Image& img) const
        {
            auto cast = (VkImage)(img);
            return (size_t)cast;
        }
    };
}

#endif