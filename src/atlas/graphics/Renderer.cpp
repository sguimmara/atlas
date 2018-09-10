#include "Drawable.hpp"
#include "Renderer.hpp"
#include "Time.hpp"
#include "RenderingOptions.hpp"
#include "SurfaceTile.hpp"
#include "Camera.hpp"
#include <numeric>
#include <sstream>

#include <glm/gtc/matrix_transform.hpp>

#define SHADER_DIR "C:/Users/sguimmara/Documents/work/c++/atlas/build/msvc/bin/shaders/"

#ifdef DEBUG
const bool enableValidationLayers = true;
#else
const bool enableValidationLayers = false;
#endif

const std::vector<const char*> validationLayers =
{
    "VK_LAYER_LUNARG_standard_validation"
};

const std::vector<const char*> deviceExtensions =
{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

namespace atlas
{
    namespace graphics
    {
        Renderer::Renderer() :
            _imageIndex(0)
        {
            _log = spdlog::stdout_color_mt("renderer");

#if DEBUG
            _log->set_level(spdlog::level::trace);
#endif

            _log->info("started");
        }

        void Renderer::Setup(GLFWwindow* window)
        {
            _window = window;

            UpdateViewportAndExtent();
            CreateInstance();
            SetupDebugCallbacks();
            CreateSurface();
            SelectGpu();
            CreateDevice();
            CreateCommandPool();
            CreateDepthResources();
            CreateSwapchain();
            CreateCommandBuffers();
            Shader::SetDirectory(SHADER_DIR);
            _log->debug("setup completed");
        }

        void Renderer::UpdateViewportAndExtent()
        {
            int w, h;
            glfwGetWindowSize(_window, &w, &h);

            _extent = vk::Extent2D(static_cast<uint32_t>(w), static_cast<uint32_t>(h));
            _viewport = vk::Viewport(0, 0, static_cast<float>(w), static_cast<float>(h));
            _viewport.setMinDepth(0);
            _viewport.setMaxDepth(1);
        }

        void Renderer::SetScene(Scene * scene)
        {
            _scene = scene;
        }

        void Renderer::DestroyScene()
        {
            _scene = nullptr;
        }

        Renderer::~Renderer()
        {
            if (enableValidationLayers)
            {
                auto vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(
                    _instance, "vkDestroyDebugReportCallbackEXT");
                vkDestroyDebugReportCallbackEXT(_instance, _debugCallback, nullptr);
            }

            DestroyScene();

            DestroySwapchain();
            DestroyCommandPool();
            DestroyDevice();
            DestroyInstance();
        }

        bool checkValidationLayerSupport()
        {
            uint32_t layerCount;
            vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

#ifdef WIN32
            // this layer causes crashes in the swapchain creation process.
            SetEnvironmentVariable("DISABLE_VK_LAYER_VALVE_steam_overlay_1", "1");
#endif

            std::vector<VkLayerProperties> availableLayers(layerCount);
            vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

            for (const char* layerName : validationLayers)
            {
                bool layerFound = false;

                for (const auto& layerProperties : availableLayers)
                {
                    if (strcmp(layerName, layerProperties.layerName) == 0)
                    {
                        layerFound = true;
                        break;
                    }
                }

                if (!layerFound)
                {
                    return false;
                }
            }

            return true;
        }

        bool checkQueuesSupport(vk::PhysicalDevice gpu, vk::SurfaceKHR surface, uint32_t* graphicsFamily, uint32_t* presentFamily)
        {
            uint32_t famCount;
            gpu.getQueueFamilyProperties(&famCount, nullptr);
            std::vector<vk::QueueFamilyProperties> families(famCount);
            gpu.getQueueFamilyProperties(&famCount, families.data());

            bool present, graphics;

            for (uint32_t i = 0; i < families.size(); ++i)
            {
                auto family = families[i];

                if (family.queueCount > 0)
                {
                    // search for a graphics queue
                    if (family.queueFlags & vk::QueueFlagBits::eGraphics)
                    {
                        graphics = true;
                        *graphicsFamily = i;
                    }

                    // search for a present queue
                    if (gpu.getSurfaceSupportKHR(i, surface) == VK_TRUE)
                    {
                        present = true;
                        *presentFamily = i;
                    }

                    if (present && graphics)
                    {
                        return true;
                    }
                }
            }

            return false;
        }

        bool Renderer::CheckExtensionSupport(vk::PhysicalDevice gpu)
        {
            uint32_t count;
            gpu.enumerateDeviceExtensionProperties(nullptr, &count, nullptr);
            std::vector<vk::ExtensionProperties> extensions(count);
            gpu.enumerateDeviceExtensionProperties(nullptr, &count, extensions.data());

            std::set<std::string> required(deviceExtensions.begin(), deviceExtensions.end());

            for (const auto& extension : extensions)
            {
                required.erase(extension.extensionName);
            }

            return required.empty();
        }

        bool checkSwapchainSupport(vk::PhysicalDevice gpu, vk::SurfaceKHR surface)
        {
            auto details = SwapChainSupportDetails::QuerySwapChainSupport(gpu, surface);
            return !details.formats.empty() && !details.presentModes.empty();
        }

        bool checkFormatSupport(vk::PhysicalDevice gpu, vk::Format format)
        {
            vk::ImageFormatProperties properties;
            auto result = gpu.getImageFormatProperties(
                format,
                vk::ImageType::e2D,
                vk::ImageTiling::eOptimal,
                vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
                vk::ImageCreateFlags(),
                &properties);

            return result != vk::Result::eErrorFormatNotSupported;
        }

        std::vector<const char*> getRequiredExtensions()
        {
            uint32_t glfwExtensionCount = 0;
            const char** glfwExtensions;
            glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

            std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

            if (enableValidationLayers)
            {
                extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
            }

            return extensions;
        }

        uint32_t Renderer::FindMemoryType(vk::PhysicalDevice gpu, uint32_t typeFilter, vk::MemoryPropertyFlags properties)
        {
            vk::PhysicalDeviceMemoryProperties memory = gpu.getMemoryProperties();

            for (uint32_t i = 0; i < memory.memoryTypeCount; i++)
            {
                if ((typeFilter & (1 << i)) && (memory.memoryTypes[i].propertyFlags & properties) == properties)
                {
                    return i;
                }
            }

            throw std::runtime_error("no memory type found");
        }

        vk::CommandBuffer Renderer::BeginSingleTimeCommand()
        {
            auto const allocInfo = vk::CommandBufferAllocateInfo()
                .setLevel(vk::CommandBufferLevel::ePrimary)
                .setCommandPool(_commandPool)
                .setCommandBufferCount(1);

            vk::CommandBuffer cmdBuffer;
            CHECK_SUCCESS(_device.allocateCommandBuffers(&allocInfo, &cmdBuffer));

            auto const beginInfo = vk::CommandBufferBeginInfo()
                .setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

            CHECK_SUCCESS(cmdBuffer.begin(&beginInfo));

            return cmdBuffer;
        }

        void Renderer::EndSingleTimeCommand(vk::CommandBuffer cmdBuffer)
        {
            cmdBuffer.end();

            auto const submit = vk::SubmitInfo()
                .setCommandBufferCount(1)
                .setPCommandBuffers(&cmdBuffer);

            _graphicsQueue.submit(1, &submit, vk::Fence());
            _graphicsQueue.waitIdle();

            _device.freeCommandBuffers(_commandPool, 1, &cmdBuffer);
        }

        void Renderer::CopyBufferToImage(uint32_t width, uint32_t height, vk::Buffer stage, vk::Image image)
        {
            vk::CommandBuffer transferBuf = BeginSingleTimeCommand();

            auto const region = vk::BufferImageCopy()
                .setBufferOffset(0)
                .setBufferRowLength(0)
                .setBufferImageHeight(0)
                .setImageSubresource(
                    vk::ImageSubresourceLayers()
                    .setAspectMask(vk::ImageAspectFlagBits::eColor)
                    .setLayerCount(1)
                    .setBaseArrayLayer(0)
                    .setMipLevel(0))
                .setImageOffset({ 0, 0, 0 })
                .setImageExtent({ width, height, 1 });

            transferBuf.copyBufferToImage(stage, image, vk::ImageLayout::eTransferDstOptimal, 1, &region);
            EndSingleTimeCommand(transferBuf);
        }

        void Renderer::TransitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
        {
            auto cmdBuffer = BeginSingleTimeCommand();

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
                throw std::invalid_argument("unsupported layout transition!");
            }

            cmdBuffer.pipelineBarrier(
                srcStage, dstStage,
                (vk::DependencyFlags)0,
                0, nullptr,
                0, nullptr,
                1, &barrier
            );

            EndSingleTimeCommand(cmdBuffer);
        }

        uint32_t Renderer::GetMemoryIndex(vk::MemoryRequirements requirements, vk::MemoryPropertyFlags flags)
        {
            return FindMemoryType(_gpu, requirements.memoryTypeBits, flags);
        }

        void Renderer::CreateBuffer(vk::DeviceSize size, void* data, vk::Buffer * buffer, vk::DeviceMemory * memory, vk::BufferUsageFlags usage)
        {
            auto const info = vk::BufferCreateInfo()
                .setSize(size)
                .setSharingMode(vk::SharingMode::eExclusive)
                .setUsage(usage);

            CHECK_SUCCESS(_device.createBuffer(&info, nullptr, buffer));

            auto const reqs = _device.getBufferMemoryRequirements(*buffer);

            auto const alloc = vk::MemoryAllocateInfo()
                .setAllocationSize(reqs.size)
                .setMemoryTypeIndex(
                    FindMemoryType(_gpu, reqs.memoryTypeBits,
                        vk::MemoryPropertyFlagBits::eHostVisible
                        | vk::MemoryPropertyFlagBits::eHostCoherent));

            void* tmp;
            CHECK_SUCCESS(_device.allocateMemory(&alloc, nullptr, memory));
            _device.bindBufferMemory(*buffer, *memory, 0);
            CHECK_SUCCESS(_device.mapMemory(*memory, 0, info.size, vk::MemoryMapFlags(), &tmp));
            memcpy(tmp, data, static_cast<size_t>(info.size));
            _device.unmapMemory(*memory);
        }

        void ScrollCallback(GLFWwindow* window, double x, double y)
        {
            auto renderer = (Renderer*)glfwGetWindowUserPointer(window);
            renderer->ProcessScrollEvents(x, y);
        }

        void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            auto renderer = (Renderer*)glfwGetWindowUserPointer(window);
            renderer->ProcessKeyEvents(key, scancode, action, mods);
        }

        void Renderer::OnWindowResized()
        {
            _pendingViewportResize = true;
        }

        void Renderer::ProcessKeyEvents(int key, int scancode, int action, int mods)
        {
            if (key == GLFW_KEY_Z && action == GLFW_PRESS)
            {
                RenderingOptions::PolygonMode = RenderingOptions::PolygonMode == vk::PolygonMode::eFill ? vk::PolygonMode::eLine : vk::PolygonMode::eFill;
            }

            if (key == GLFW_KEY_S && action == GLFW_PRESS)
            {
                static_cast<SurfaceTile*>(_scene->root()->get_child(0)->get_child(0))->Split();
            }
            if (key == GLFW_KEY_R && action == GLFW_PRESS)
            {
                static_cast<SurfaceTile*>(_scene->root()->get_child(0)->get_child(0))->Reduce();
            }
        }

        void Renderer::ProcessScrollEvents(double x, double y)
        {
            Camera::main->SetFov(Camera::main->fov() + static_cast<float>(y * 0.1));
        }

        void WindowResizedCallback(GLFWwindow* window, int width, int height)
        {
            auto renderer = reinterpret_cast<Renderer*>(glfwGetWindowUserPointer(window));

            renderer->OnWindowResized();
        }

        void Renderer::ResizeViewport()
        {
            _device.waitIdle();
            _presentQueue.waitIdle();

            DestroySwapchain();
            DestroyDepthResources();

            UpdateViewportAndExtent();

            CreateDepthResources();
            CreateSwapchain();

            for (auto node : *_scene)
            {
                node->SendSignal(Signal::WindowResized);
            }

            _imageIndex = 0;
        }

        void Renderer::Run()
        {
            _log->debug("entered main loop");

            glfwSetKeyCallback(_window, KeyCallback);
            glfwSetScrollCallback(_window, ScrollCallback);
            glfwSetFramebufferSizeCallback(_window, WindowResizedCallback);

            while (!glfwWindowShouldClose(_window))
            {
                glfwPollEvents();

                auto start = std::chrono::steady_clock::now();

                if (_pendingViewportResize)
                {
                    ResizeViewport();
                    _pendingViewportResize = false;
                }

                RenderFrame();

                auto end = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();

                PostFrame(elapsed);
            }

            _log->debug("exited main loop");
        }

        void Renderer::PostFrame(double elapsedSeconds)
        {
            Time::dt = elapsedSeconds;
            Time::elapsed += elapsedSeconds;

            if (_slidingElapsedTime.size() == 0)
            {
                _slidingElapsedTime.resize(50);
            }

            _slidingElapsedTime[_slidingElapsedTimeIndex] = elapsedSeconds;
            (++_slidingElapsedTimeIndex) %= _slidingElapsedTime.size();

            double avg = std::accumulate(_slidingElapsedTime.begin(), _slidingElapsedTime.end(), 0.0, std::plus<double>()) 
                        / static_cast<double>(_slidingElapsedTime.size());

            std::stringstream title;
            title << APP_NAME " - ";
            title << std::round(1 / avg);
            title << " FPS";
            title << " - ";
            title << _scene->root()->size();
            title << " objects";

            glfwSetWindowTitle(_window, title.str().c_str());
        }

        void Renderer::CreateInstance()
        {
            if (enableValidationLayers && !checkValidationLayerSupport())
            {
                throw std::runtime_error("validation layers requested, but not available!");
            }

            auto const app = vk::ApplicationInfo()
                .setPApplicationName(APP_NAME)
                .setApplicationVersion(0)
                .setPEngineName(APP_NAME)
                .setApiVersion(VK_API_VERSION_1_0);

            _enabledExtensions = getRequiredExtensions();
            _log->debug("enabled extensions:", _enabledExtensions.size());
            for (const auto ex : _enabledExtensions)
            {
                _log->debug("\t{0}", ex);
            }

            auto inst = vk::InstanceCreateInfo()
                .setPApplicationInfo(&app)
                .setEnabledExtensionCount(static_cast<uint32_t>(_enabledExtensions.size()))
                .setPpEnabledExtensionNames(_enabledExtensions.data());
            if (enableValidationLayers)
            {
                inst.setEnabledLayerCount(static_cast<uint32_t>(validationLayers.size()))
                    .setPpEnabledLayerNames(validationLayers.data());
            }

            _log->debug("enabled layers:", validationLayers.size());
            for (const auto vl : validationLayers)
            {
                _log->debug("\t{0}", vl);
            }

            auto result = vk::createInstance(&inst, nullptr, &_instance);
            VERIFY(result == vk::Result::eSuccess);
            _log->debug("instance created");
        }

        void Renderer::DestroyInstance()
        {
            _instance.destroy(nullptr);
            _log->debug("instance destroyed");
        }

        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugReportFlagsEXT flags,
            VkDebugReportObjectTypeEXT objType,
            uint64_t obj, size_t location, int32_t code,
            const char* layerPrefix, const char* msg, void* userData)
        {
            spdlog::get("renderer")->error(msg);
            std::terminate();
            return VK_FALSE;
        }

        VkResult createDebugReportCallbackEXT(
            VkInstance instance,
            const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
            const VkAllocationCallbacks* pAllocator,
            VkDebugReportCallbackEXT* pCallback)
        {
            auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
            if (func != nullptr)
            {
                return func(instance, pCreateInfo, pAllocator, pCallback);
            }
            else
            {
                return VK_ERROR_EXTENSION_NOT_PRESENT;
            }
        }

        void Renderer::SetupDebugCallbacks()
        {
            if (!enableValidationLayers) return;

            VkDebugReportCallbackCreateInfoEXT createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
            createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
            createInfo.pfnCallback = debugCallback;

            VK_CHECK_RESULT(createDebugReportCallbackEXT(_instance, &createInfo, nullptr, &_debugCallback));

            _log->debug("setup debug callback");
        }

        void Renderer::CreateSurface()
        {
            VK_CHECK_RESULT(glfwCreateWindowSurface(_instance, _window, nullptr, (VkSurfaceKHR*)&_surface));
            _log->debug("surface created");
        }

        void Renderer::SelectGpu()
        {
            uint32_t count = 0;

            _instance.enumeratePhysicalDevices(&count, nullptr);
            std::vector<vk::PhysicalDevice> gpus(count);
            _instance.enumeratePhysicalDevices(&count, gpus.data());
            _log->debug("found {0} GPU(s)", count);

            if (count == 0)
            {
                throw std::runtime_error("could not find any GPU with Vulkan support.");
            }

            bool gpuFound = false;
            for (auto& gpu : gpus)
            {
                uint32_t present = 0;
                uint32_t graphics = 0;
                if (checkQueuesSupport(gpu, _surface, &graphics, &present)
                    && CheckExtensionSupport(gpu)
                    && checkSwapchainSupport(gpu, _surface)
                    && checkFormatSupport(gpu, vk::Format::eR8G8B8A8Unorm))
                {
                    _gpu = gpu;
                    _presentFamily = present;
                    _graphicsFamily = graphics;
                    gpuFound = true;
                    break;
                }
            }

            if (!gpuFound)
            {
                throw std::runtime_error("could not find a compatible GPU");
            }

            vk::PhysicalDeviceProperties gpuProps;
            _gpu.getProperties(&gpuProps);

            _log->info("GPU:        {0}", gpuProps.deviceName);
            int driver = gpuProps.driverVersion;
            _log->info("GPU driver: {0}.{1}.{2}",
                VK_VERSION_MAJOR(driver),
                VK_VERSION_MINOR(driver),
                VK_VERSION_PATCH(driver));

            int apiVer = gpuProps.apiVersion;
            _log->info("Vulkan API: {0}.{1}.{2}",
                VK_VERSION_MAJOR(apiVer),
                VK_VERSION_MINOR(apiVer),
                VK_VERSION_PATCH(apiVer));
        }

#define CHECK_FEATURE(feat) \
if (!features.feat) \
{             \
        _log->critical("missing GPU feature : " #feat); \
        throw std::runtime_error("missing GPU feature"); \
}

        void Renderer::CheckFeatures()
        {
            const vk::PhysicalDeviceFeatures features = _gpu.getFeatures();

            CHECK_FEATURE(fillModeNonSolid);
            CHECK_FEATURE(samplerAnisotropy);
            CHECK_FEATURE(multiViewport);
        }

        void Renderer::CreateDevice()
        {
            std::set<uint32_t> uniqueQueueFamilies = { _presentFamily, _graphicsFamily };
            std::vector<vk::DeviceQueueCreateInfo> queues;

            float priority = 1.0f;
            for (uint32_t queueFamily : uniqueQueueFamilies)
            {
                auto const qInfo = vk::DeviceQueueCreateInfo()
                    .setPQueuePriorities(&priority)
                    .setQueueCount(1)
                    .setQueueFamilyIndex(queueFamily);
                queues.push_back(qInfo);
            }

            CheckFeatures();

            auto const features = vk::PhysicalDeviceFeatures()
                .setFillModeNonSolid(VK_TRUE)
                .setSamplerAnisotropy(VK_TRUE)
                .setMultiViewport(VK_TRUE);

            auto info = vk::DeviceCreateInfo()
                .setQueueCreateInfoCount(static_cast<uint32_t>(queues.size()))
                .setPQueueCreateInfos(queues.data())
                .setEnabledExtensionCount(static_cast<uint32_t>(deviceExtensions.size()))
                .setPpEnabledExtensionNames(deviceExtensions.data())
                .setPEnabledFeatures(&features);

            if (enableValidationLayers)
            {
                info.setEnabledLayerCount(static_cast<uint32_t>(validationLayers.size()))
                    .setPpEnabledLayerNames(validationLayers.data());
            }

            VERIFY(_gpu.createDevice(&info, nullptr, &_device) == vk::Result::eSuccess);
            _log->debug("created device");
            _device.getQueue(_presentFamily, 0, &_presentQueue);
            _device.getQueue(_graphicsFamily, 0, &_graphicsQueue);
        }

        void Renderer::DestroyDevice()
        {
            _device.destroy();
            _log->debug("destroyed device");
        }

        vk::PresentModeKHR Renderer::ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR> availablePresentModes)
        {
            vk::PresentModeKHR bestMode = vk::PresentModeKHR::eFifo;

            for (const auto& availablePresentMode : availablePresentModes)
            {
                if (availablePresentMode == vk::PresentModeKHR::eMailbox)
                {
                    return availablePresentMode;
                }
                else if (availablePresentMode == vk::PresentModeKHR::eImmediate)
                {
                    bestMode = availablePresentMode;
                }
            }

            return bestMode;
        }

        vk::Extent2D Renderer::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, GLFWwindow* window)
        {
            if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
            {
                return capabilities.currentExtent;
            }
            else
            {
                int width, height;
                glfwGetFramebufferSize(window, &width, &height);

                vk::Extent2D actualExtent =
                {
                    static_cast<uint32_t>(width),
                    static_cast<uint32_t>(height)
                };

                actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
                actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

                return actualExtent;
            }
        }

        vk::SurfaceFormatKHR Renderer::ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
        {
            if (availableFormats.size() == 1 && availableFormats[0].format == vk::Format::eUndefined)
            {
                return { vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear };
            }

            for (const auto& availableFormat : availableFormats)
            {
                if (availableFormat.format == vk::Format::eB8G8R8A8Unorm && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
                {
                    return availableFormat;
                }
            }

            return availableFormats[0];
        }

        void Renderer::CreateSwapchain()
        {
            SwapChainSupportDetails swapChainSupport = SwapChainSupportDetails::QuerySwapChainSupport(_gpu, _surface);
            _swapchainFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);

            auto const colorAttachment = vk::AttachmentDescription()
                .setFormat(_swapchainFormat.format)
                .setSamples(vk::SampleCountFlagBits::e1)
                .setLoadOp(vk::AttachmentLoadOp::eClear)
                .setStoreOp(vk::AttachmentStoreOp::eStore)
                .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
                .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
                .setInitialLayout(vk::ImageLayout::eUndefined)
                .setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

            auto const depthAttachment = vk::AttachmentDescription()
                .setFormat(vk::Format::eD24UnormS8Uint)
                .setSamples(vk::SampleCountFlagBits::e1)
                .setLoadOp(vk::AttachmentLoadOp::eClear)
                .setStoreOp(vk::AttachmentStoreOp::eStore)
                .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
                .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
                .setInitialLayout(vk::ImageLayout::eUndefined)
                .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

            auto const colorAttachmentRef = vk::AttachmentReference()
                .setAttachment(0)
                .setLayout(vk::ImageLayout::eColorAttachmentOptimal);

            auto const depthAttachmentRef = vk::AttachmentReference()
                .setAttachment(1)
                .setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

            auto const subpass = vk::SubpassDescription()
                .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
                .setColorAttachmentCount(1)
                .setPColorAttachments(&colorAttachmentRef)
                .setPDepthStencilAttachment(&depthAttachmentRef);

            auto const dependency = vk::SubpassDependency()
                .setSrcSubpass(VK_SUBPASS_EXTERNAL)
                .setDstSubpass(0)
                .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
                .setSrcAccessMask((vk::AccessFlagBits)0)
                .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
                .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite);

            std::vector<vk::AttachmentDescription> attachments = { colorAttachment, depthAttachment };

            auto const info = vk::RenderPassCreateInfo()
                .setAttachmentCount(static_cast<uint32_t>(attachments.size()))
                .setPAttachments(attachments.data())
                .setSubpassCount(1)
                .setPSubpasses(&subpass)
                .setDependencyCount(1)
                .setPDependencies(&dependency);

            CHECK_SUCCESS(_device.createRenderPass(&info, nullptr, &_renderPass));
            _log->debug("created render pass");

            vk::PresentModeKHR mode = ChooseSwapPresentMode(swapChainSupport.presentModes);
            _extent = ChooseSwapExtent(swapChainSupport.capabilities, _window);
            uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
            if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
            {
                imageCount = swapChainSupport.capabilities.maxImageCount;
            }

            _log = spdlog::get("renderer");
            _log->debug("swapchain image count :  {0:d}", imageCount);

            auto swapChainInfo = vk::SwapchainCreateInfoKHR()
                .setSurface(_surface)
                .setMinImageCount(imageCount)
                .setImageFormat(_swapchainFormat.format)
                .setImageColorSpace(_swapchainFormat.colorSpace)
                .setImageExtent(_extent)
                .setImageArrayLayers(1)
                .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
                .setPreTransform(swapChainSupport.capabilities.currentTransform)
                .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
                .setPresentMode(mode)
                .setClipped(VK_TRUE);

            uint32_t queueFamilyIndices[] = { _graphicsFamily, _presentFamily };

            if (_graphicsFamily != _presentFamily)
            {
                swapChainInfo.imageSharingMode = vk::SharingMode::eConcurrent;
                swapChainInfo.queueFamilyIndexCount = 2;
                swapChainInfo.pQueueFamilyIndices = queueFamilyIndices;
            }
            else
            {
                swapChainInfo.imageSharingMode = vk::SharingMode::eExclusive;
            }

            _device.createSwapchainKHR(&swapChainInfo, nullptr, &_swapchain);
            _device.getSwapchainImagesKHR(_swapchain, &imageCount, nullptr);

            std::vector<vk::Image> images;
            images.resize(imageCount);
            _device.getSwapchainImagesKHR(_swapchain, &imageCount, images.data());

            for (uint32_t i = 0; i < images.size(); i++)
            {
                RenderTarget target;

                target.device = _device;
                target.index = i;
                target.image = images[i];

                auto const semaphoreInfo = vk::SemaphoreCreateInfo();
                _device.createSemaphore(&semaphoreInfo, nullptr, &target.imageAcquired);
                _device.createSemaphore(&semaphoreInfo, nullptr, &target.renderComplete);
                _device.createSemaphore(&semaphoreInfo, nullptr, &target.imageOwnership);

                auto imgViewInfo = vk::ImageViewCreateInfo()
                    .setImage(target.image)
                    .setViewType(vk::ImageViewType::e2D)
                    .setFormat(_swapchainFormat.format)
                    .setComponents(vk::ComponentMapping())
                    .setSubresourceRange(
                        vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));

                CHECK_SUCCESS(_device.createImageView(&imgViewInfo, nullptr, &target.view));

                std::array<vk::ImageView, 2> attachments = { target.view, _depthImageView };
                auto const fbinfo = vk::FramebufferCreateInfo()
                    .setRenderPass(_renderPass)
                    .setAttachmentCount(static_cast<uint32_t>(attachments.size()))
                    .setPAttachments(attachments.data())
                    .setWidth(_extent.width)
                    .setHeight(_extent.height)
                    .setLayers(1);

                CHECK_SUCCESS(_device.createFramebuffer(&fbinfo, nullptr, &target.framebuffer));

                _renderTargets.push_back(target);
            }

            _log->debug("created swapchain");
        }

        void Renderer::DestroySwapchain()
        {
            for (auto target : _renderTargets)
            {
                _device.destroySemaphore(target.imageAcquired);
                _device.destroySemaphore(target.renderComplete);
                _device.destroySemaphore(target.imageOwnership);

                _device.destroyFramebuffer(target.framebuffer);
                _device.destroyImageView(target.view);
            }

            _renderTargets.clear();

            _device.destroySwapchainKHR(_swapchain, nullptr);
            _log->debug("destroyed swapchain");

            _device.destroyRenderPass(_renderPass, nullptr);
            _log->debug("destroyed render pass");
        }

        void Renderer::CreateDepthResources()
        {
            auto const format = vk::Format::eD24UnormS8Uint;

            auto const imageInfo = vk::ImageCreateInfo()
                .setFormat(format)
                .setUsage(vk::ImageUsageFlagBits::eDepthStencilAttachment)
                .setTiling(vk::ImageTiling::eOptimal)
                .setMipLevels(1)
                .setImageType(vk::ImageType::e2D)
                .setArrayLayers(1)
                .setSharingMode(vk::SharingMode::eExclusive)
                .setSamples(vk::SampleCountFlagBits::e1)
                .setExtent(vk::Extent3D(_extent, 1));

            CHECK_SUCCESS(_device.createImage(&imageInfo, nullptr, &_depthImage));

            vk::MemoryRequirements memReq = _device.getImageMemoryRequirements(_depthImage);

            auto const allocInfo = vk::MemoryAllocateInfo()
                .setAllocationSize(memReq.size)
                .setMemoryTypeIndex(GetMemoryIndex(memReq, vk::MemoryPropertyFlagBits::eDeviceLocal));

            CHECK_SUCCESS(_device.allocateMemory(&allocInfo, nullptr, &_depthImageMemory));

            _device.bindImageMemory(_depthImage, _depthImageMemory, 0);

            auto const imageViewInfo = vk::ImageViewCreateInfo()
                .setImage(_depthImage)
                .setFormat(format)
                .setViewType(vk::ImageViewType::e2D)
                .setSubresourceRange(
                    vk::ImageSubresourceRange()
                    .setAspectMask(vk::ImageAspectFlagBits::eDepth)
                    .setBaseMipLevel(0)
                    .setLevelCount(1)
                    .setBaseArrayLayer(0)
                    .setLayerCount(1));

            TransitionImageLayout(_depthImage, format, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);

            CHECK_SUCCESS(_device.createImageView(&imageViewInfo, nullptr, &_depthImageView));
        }

        void Renderer::DestroyDepthResources()
        {
            _device.destroyImageView(_depthImageView);
            _device.destroyImage(_depthImage);
            _device.freeMemory(_depthImageMemory);
        }

        void Renderer::CreateCommandPool()
        {
            auto const info = vk::CommandPoolCreateInfo()
                .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

            _device.createCommandPool(&info, nullptr, &_commandPool);
            _log->debug("created command pool");
        }

        void Renderer::DestroyCommandPool()
        {
            _device.destroyCommandPool(_commandPool, nullptr);
            _log->debug("destroyed command pool");
        }

        void Renderer::CreateCommandBuffers()
        {
            _commandBuffers.resize(_renderTargets.size());

            auto const allocInfo = vk::CommandBufferAllocateInfo()
                .setCommandBufferCount(static_cast<uint32_t>(_commandBuffers.size()))
                .setCommandPool(_commandPool)
                .setLevel(vk::CommandBufferLevel::ePrimary);

            auto result = _device.allocateCommandBuffers(&allocInfo, _commandBuffers.data());
            VERIFY(result == vk::Result::eSuccess);
        }

        void Renderer::UpdateCommandBuffers()
        {
            ApplyTransformations(*_scene);

            vk::ClearValue const clearValues[2] = {
                vk::ClearColorValue(std::array<float, 4>({ { 30.0f / 255, 65.0f / 255, 84.0f / 255 , 1 } })),
                vk::ClearDepthStencilValue(1.0f, 0u) };

            auto const scissor = vk::Rect2D({ 0, 0 }, _extent);

            for (uint32_t i = 0; i < _commandBuffers.size(); i++)
            {
                auto buffer = _commandBuffers[i];

                auto const begin = vk::CommandBufferBeginInfo()
                    .setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse);

                auto result = buffer.begin(&begin);
                VERIFY(result == vk::Result::eSuccess);
                {
                    auto const renderPassInfo = vk::RenderPassBeginInfo()
                        .setRenderPass(_renderPass)
                        .setFramebuffer(_renderTargets[i].framebuffer)
                        .setRenderArea(vk::Rect2D(vk::Offset2D(0, 0), _extent))
                        .setClearValueCount(2)
                        .setPClearValues(clearValues);

                    for (auto camera : _scene->cameras())
                    {
                        buffer.setViewport(0, 1, &camera->viewport());
                        buffer.setScissor(0, 1, &scissor);
                        buffer.beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);
                        {
                            RenderScene(*_scene, *camera, buffer);
                        }
                        buffer.endRenderPass();
                    }
                }
                buffer.end();
            }
        }

        void Renderer::ApplyTransformations(Scene& scene)
        {
            for (auto const node : scene)
            {
                if (node->parent() != nullptr)
                {
                    node->setTransform(node->parent()->transform() * node->localTransform());
                }
            }
        }

        void Renderer::RenderScene(Scene& scene, Camera& camera, vk::CommandBuffer& cmdBuffer)
        {
            auto uctx = UpdateContext{ &camera };
            auto ctx = DrawContext{ cmdBuffer, camera.transform(), camera.projection() };
            for (auto const node : scene)
            {
                node->Update(uctx);
                if (node->flags() & (int)NodeFlags::Drawable)
                {
                    static_cast<Drawable*>(node)->Draw(ctx);
                }
            }
        }

        void Renderer::SubmitFrame(RenderTarget frame, vk::CommandBuffer buffer)
        {
            vk::PipelineStageFlags const pipeStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
            auto const info = vk::SubmitInfo()
                .setWaitSemaphoreCount(1)
                .setPWaitSemaphores(&frame.imageAcquired)
                .setSignalSemaphoreCount(1)
                .setPSignalSemaphores(&frame.renderComplete)
                .setCommandBufferCount(1)
                .setPCommandBuffers(&buffer)
                .setPWaitDstStageMask(&pipeStage);

            CHECK_SUCCESS(_graphicsQueue.submit(1, &info, vk::Fence()));

            bool separateQueues = _presentFamily != _graphicsFamily;
            if (separateQueues)
            {
                auto const presentInfo = vk::SubmitInfo()
                    .setPWaitDstStageMask(&pipeStage)
                    .setWaitSemaphoreCount(1)
                    .setPWaitSemaphores(&frame.renderComplete)
                    .setCommandBufferCount(0)
                    .setSignalSemaphoreCount(1)
                    .setPSignalSemaphores(&frame.imageOwnership);

                CHECK_SUCCESS(_presentQueue.submit(1, &presentInfo, vk::Fence()));
            }

            _device.waitIdle();

            auto const presentInfo = vk::PresentInfoKHR()
                .setWaitSemaphoreCount(1)
                .setPWaitSemaphores(separateQueues ? &frame.imageOwnership : &frame.renderComplete)
                .setSwapchainCount(1)
                .setPSwapchains(&_swapchain)
                .setPImageIndices(&frame.index);

            CHECK_SUCCESS(_presentQueue.presentKHR(&presentInfo));

            _presentQueue.waitIdle();
        }

        void Renderer::RenderFrame()
        {
            assert(_extent.width > 0);
            assert(_extent.height > 0);

            auto cameras = _scene->cameras();
            if (cameras.empty())
            {
                _log->error("no camera to render");
            }
            else
            {
                RenderTarget& img = _renderTargets[_imageIndex];
                CHECK_SUCCESS(_device.acquireNextImageKHR(_swapchain, _swapWaitTimeout, img.imageAcquired, vk::Fence(), &_imageIndex));

                UpdateCommandBuffers();
                SubmitFrame(img, _commandBuffers[_imageIndex]);

                ++_imageIndex;
                _imageIndex %= _renderTargets.size();

                _graphicsQueue.waitIdle();
                _device.waitIdle();
            }
        }

    }
}