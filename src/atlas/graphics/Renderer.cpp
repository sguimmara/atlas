#include "Renderer.hpp"

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
        Renderer::Renderer()
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

            CreateInstance();
            SetupDebugCallbacks();
            CreateSurface();
            SelectGpu();
            CreateDevice();
            CreateCommandPool();
            CreateFramebuffer();
            CreateImages();
        }

        Renderer::~Renderer()
        {
            if (enableValidationLayers)
            {
                auto vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(
                    _instance, "vkDestroyDebugReportCallbackEXT");
                vkDestroyDebugReportCallbackEXT(_instance, _debugCallback, nullptr);
            }

            DestroyFrameBuffer();
            DestroyCommandPool();
            DestroyDevice();
            DestroyInstance();

            for (auto img : _images)
            {
                img.destroy();
            }
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

        void Renderer::Run()
        {
            assert(_framebuffer);

            _log->debug("entered main loop");

            while (!glfwWindowShouldClose(_window))
            {
                glfwPollEvents();
                RenderFrame();

                std::this_thread::sleep_for(std::chrono::milliseconds(100));

                _graphicsQueue.waitIdle();
                _device.waitIdle();
            }

            _log->debug("exited main loop");
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

            auto info = vk::DeviceCreateInfo()
                .setQueueCreateInfoCount(static_cast<uint32_t>(queues.size()))
                .setPQueueCreateInfos(queues.data())
                .setEnabledExtensionCount(static_cast<uint32_t>(deviceExtensions.size()))
                .setPpEnabledExtensionNames(deviceExtensions.data());

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
        
        void Renderer::CreateFramebuffer()
        {
            assert(!_framebuffer);
            _framebuffer = new Framebuffer(_window, _device, _gpu, _surface, _graphicsFamily, _presentFamily);
        }

        void Renderer::DestroyFrameBuffer()
        {
            assert(_framebuffer);
            delete _framebuffer;
            _framebuffer = nullptr;
        }
        
        void Renderer::CreateImages()
        {
            _images.clear();
            for (size_t i = 0; i < _imageLag; i++)
            {
                _images.emplace_back(_device);
            }
        }

        void Renderer::RenderFrame()
        {
            ImageStruct& img = _images[_imageIndex];

            ++_imageIndex;
            _imageIndex %= 2;

            vk::Result result;
            uint32_t target = _framebuffer->AcquireImage(img.imageAcquired);

            // Render code

            vk::PipelineStageFlags const pipeStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
            auto const info = vk::SubmitInfo()
                .setWaitSemaphoreCount(1)
                .setPWaitSemaphores(&img.imageAcquired)
                .setSignalSemaphoreCount(1)
                .setPSignalSemaphores(&img.renderComplete)
                .setCommandBufferCount(0)
                .setPWaitDstStageMask(&pipeStage);

            result = _graphicsQueue.submit(1, &info, vk::Fence());
            VERIFY(result == vk::Result::eSuccess);

            bool separateQueues = _presentFamily != _graphicsFamily;
            if (separateQueues)
            {
                auto const presentInfo = vk::SubmitInfo()
                    .setPWaitDstStageMask(&pipeStage)
                    .setWaitSemaphoreCount(1)
                    .setPWaitSemaphores(&img.renderComplete)
                    .setCommandBufferCount(0)
                    .setSignalSemaphoreCount(1)
                    .setPSignalSemaphores(&img.imageOwnership);

                result = _presentQueue.submit(1, &presentInfo, vk::Fence());
                VERIFY(result == vk::Result::eSuccess);
            }

            _device.waitIdle();

            _framebuffer->PresentImage(target, separateQueues ? img.imageOwnership : img.renderComplete);
        }
    }
}