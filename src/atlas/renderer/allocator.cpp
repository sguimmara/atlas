#include "allocator.hpp"

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

using namespace atlas::renderer;

vk::Device Allocator::_device = vk::Device();
vk::CommandPool Allocator::_commandPool = vk::CommandPool();
vk::Queue Allocator::_transferQueue = vk::Queue();
std::shared_ptr<spdlog::logger> Allocator::_log = nullptr;
vk::DescriptorPool Allocator::_descriptorPool = nullptr;
vk::PhysicalDevice Allocator::_physicalDevice = vk::PhysicalDevice();
std::unordered_map<vk::Buffer, VmaAllocation> Allocator::_allocatedBuffers = std::unordered_map<vk::Buffer, VmaAllocation>();
std::unordered_map<vk::Image, VmaAllocation> Allocator::_allocatedImages = std::unordered_map<vk::Image, VmaAllocation>();
VmaAllocator Allocator::_vma = VmaAllocator();

void Allocator::init(vk::Device device, vk::PhysicalDevice gpu, uint32_t transferFamily, vk::Queue transfer)
{
    _log = spdlog::get("allocator");
    if (!_log)
    {
        _log = spdlog::stdout_color_mt("allocator");
    }

    _log->info("initializing");

    _transferQueue = transfer;
    _device = device;
    _physicalDevice = gpu;
    _commandPool = device.createCommandPool(vk::CommandPoolCreateInfo()
        .setQueueFamilyIndex(transferFamily));

    VmaAllocatorCreateInfo info = {};
    info.device = device;
    info.physicalDevice = gpu;

    vmaCreateAllocator(&info, &_vma);

    createDescriptorPool();

    _log->trace("VMA allocator successfully created");
}

void Allocator::terminate()
{
    vmaDestroyAllocator(_vma);
    _device.destroyCommandPool(_commandPool);
    _device.destroyDescriptorPool(_descriptorPool);
    _log->info("terminated");
}

vk::CommandBuffer Allocator::beginSingleTimeCommand()
{
    auto const allocInfo = vk::CommandBufferAllocateInfo()
        .setLevel(vk::CommandBufferLevel::ePrimary)
        .setCommandPool(_commandPool)
        .setCommandBufferCount(1);

    auto cmdBuffers = _device.allocateCommandBuffers(allocInfo);

    auto const beginInfo = vk::CommandBufferBeginInfo()
        .setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    check(cmdBuffers[0].begin(&beginInfo));

    return cmdBuffers[0];
}

void Allocator::endSingleTimeCommand(vk::CommandBuffer cmdBuffer)
{
    cmdBuffer.end();

    auto const submit = vk::SubmitInfo()
        .setCommandBufferCount(1)
        .setPCommandBuffers(&cmdBuffer);

    _transferQueue.submit(1, &submit, vk::Fence());
    _transferQueue.waitIdle();
    _device.freeCommandBuffers(_commandPool, 1, &cmdBuffer);
}

vk::Buffer Allocator::getBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage)
{
    VkBuffer buffer;
    VmaAllocation alloc;

    VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    bufferInfo.size = size;
    bufferInfo.usage = (VkBufferUsageFlags)usage;

    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

    vmaCreateBuffer(_vma, &bufferInfo, &allocInfo, &buffer, &alloc, nullptr);

    _allocatedBuffers.insert({ buffer, alloc });

    return buffer;
}

vk::Buffer Allocator::getStagingBuffer(vk::DeviceSize size)
{
    //return getBuffer(size, vk::BufferUsageFlagBits::eTransferSrc);

    VkBuffer buffer;
    VmaAllocation alloc;

    VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    bufferInfo.size = size;
    bufferInfo.usage = (VkBufferUsageFlags)vk::BufferUsageFlagBits::eTransferSrc;

    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

    vmaCreateBuffer(_vma, &bufferInfo, &allocInfo, &buffer, &alloc, nullptr);

    _allocatedBuffers.insert({ buffer, alloc });

    return buffer;
}

vk::Image Allocator::getImage(vk::Extent2D extents, vk::Format format, vk::ImageUsageFlags usage, vk::ImageTiling tiling)
{
    auto const imageInfo = vk::ImageCreateInfo()
        .setImageType(vk::ImageType::e2D)
        .setExtent(vk::Extent3D(extents, 1))
        .setFormat(format)
        .setTiling(tiling)
        .setMipLevels(1)
        .setArrayLayers(1)
        .setUsage(usage) // vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst
        .setSharingMode(vk::SharingMode::eExclusive)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setSamples(vk::SampleCountFlagBits::e1);

    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    VkImage image;
    VmaAllocation alloc;
    auto info = (VkImageCreateInfo)imageInfo;
    vmaCreateImage(_vma, &info, &allocInfo, &image, &alloc, nullptr);

    _allocatedImages.insert({ image, alloc });

    return image;
}

void Allocator::transition(vk::Image image, vk::Format format,
    vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
{
    auto cmdBuffer = beginSingleTimeCommand();

    vk::ImageAspectFlags aspectMask = vk::ImageAspectFlagBits::eColor;
    if (newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
    {
        aspectMask = vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
    }

    auto barrier = vk::ImageMemoryBarrier()
        .setOldLayout(oldLayout)
        .setNewLayout(newLayout)
        .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
        .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
        .setImage(image)
        .setSubresourceRange(vk::ImageSubresourceRange()
            .setAspectMask(aspectMask)
            .setBaseMipLevel(0)
            .setLevelCount(1)
            .setBaseArrayLayer(0)
            .setLayerCount(1));

    vk::PipelineStageFlags srcStage;
    vk::PipelineStageFlags dstStage;

    if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
    {
        barrier.setSrcAccessMask((vk::AccessFlags)0);
        barrier.setDstAccessMask(vk::AccessFlagBits::eTransferWrite);

        srcStage = vk::PipelineStageFlagBits::eTopOfPipe;
        dstStage = vk::PipelineStageFlagBits::eTransfer;
    }
    else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
    {
        barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite);
        barrier.setDstAccessMask(vk::AccessFlagBits::eShaderRead);

        srcStage = vk::PipelineStageFlagBits::eTransfer;
        dstStage = vk::PipelineStageFlagBits::eFragmentShader;
    }
    else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
    {
        barrier.setSrcAccessMask((vk::AccessFlags)0);
        barrier.setDstAccessMask(vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite);
        srcStage = vk::PipelineStageFlagBits::eTopOfPipe;
        dstStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
    }
    else
    {
        _log->error("unsupported layout transition: {0} -> {1}", vk::to_string(oldLayout), vk::to_string(newLayout));
        throw std::invalid_argument("unsupported layout transition!");
    }

    cmdBuffer.pipelineBarrier(
        srcStage, dstStage,
        (vk::DependencyFlags)0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    endSingleTimeCommand(cmdBuffer);
}

void Allocator::copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height)
{
    vk::CommandBuffer cmd = beginSingleTimeCommand();

    vk::BufferImageCopy region = {};
    region.setBufferOffset(0)
        .setBufferRowLength(0)
        .setBufferImageHeight(0)
        .setImageSubresource(
            vk::ImageSubresourceLayers()
            .setAspectMask(vk::ImageAspectFlagBits::eColor)
            .setMipLevel(0)
            .setBaseArrayLayer(0)
            .setLayerCount(1))
        .setImageOffset({ 0, 0, 0 })
        .setImageExtent({ width, height, 1 });

    cmd.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, 1, &region);

    endSingleTimeCommand(cmd);
}

void Allocator::write(vk::Buffer buf, void* data, size_t size, size_t offset)
{
    auto const block = _allocatedBuffers[buf];

    void *target;
    vmaMapMemory(_vma, block, &target);
    memcpy((char*)target + offset, data, size);
    vmaUnmapMemory(_vma, block);
}

void Allocator::free(vk::Buffer buf)
{
    assert(_allocatedBuffers.count(buf) != 0);

    auto const block = _allocatedBuffers[buf];
    vmaDestroyBuffer(_vma, buf, block);

    _allocatedBuffers.erase(buf);
}

void Allocator::free(vk::Image img)
{
    assert(_allocatedImages.count(img) != 0);

    auto const block = _allocatedImages[img];
    vmaDestroyImage(_vma, img, block);

    _allocatedImages.erase(img);
}

vk::DescriptorSet Allocator::getDescriptorSet(vk::DescriptorSetLayout layout)
{
    return _device.allocateDescriptorSets(vk::DescriptorSetAllocateInfo()
        .setDescriptorPool(_descriptorPool)
        .setDescriptorSetCount(1)
        .setPSetLayouts(&layout))[0];
}

void Allocator::free(vk::DescriptorSet set)
{
    _device.freeDescriptorSets(_descriptorPool, 1, &set);
}

void Allocator::createDescriptorPool()
{
    auto sizes = std::vector<vk::DescriptorPoolSize>();

    sizes.push_back(vk::DescriptorPoolSize()
        .setType(vk::DescriptorType::eCombinedImageSampler)
        .setDescriptorCount(1024));

    sizes.push_back(vk::DescriptorPoolSize()
        .setType(vk::DescriptorType::eUniformBuffer)
        .setDescriptorCount(1024));

    sizes.push_back(vk::DescriptorPoolSize()
        .setType(vk::DescriptorType::eUniformBufferDynamic)
        .setDescriptorCount(256));

    _descriptorPool = _device.createDescriptorPool(vk::DescriptorPoolCreateInfo()
        .setPoolSizeCount(static_cast<uint32_t>(sizes.size()))
        .setMaxSets(1024)
        .setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
        .setPPoolSizes(sizes.data()));
}
