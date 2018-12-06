#include "instance.hpp"
#include <filesystem>

using namespace atlas::renderer;

#if DEBUG
const bool EnableValidationLayers = true;
#else
const bool EnableValidationLayers = false;
#endif
const std::vector<const char*> validationLayersNames =
{
    "VK_LAYER_LUNARG_standard_validation"
};

std::shared_ptr<spdlog::logger> Instance::_log = nullptr;
vk::Instance Instance::_instance = vk::Instance();
vk::SurfaceKHR Instance::_surface = vk::SurfaceKHR();
vk::SurfaceFormatKHR Instance::_surfaceFormat = vk::SurfaceFormatKHR();
vk::PhysicalDevice Instance::physicalDevice = vk::PhysicalDevice();
vk::RenderPass Instance::renderPass = vk::RenderPass();
vk::Device Instance::device = vk::Device();
GLFWwindow* Instance::_window = nullptr;
uint32_t Instance::_exitCode = 0;
std::unique_ptr<Context> Instance::_context = nullptr;
vk::Queue Instance::graphicsQueue = vk::Queue();
vk::Queue Instance::presentQueue = vk::Queue();
vk::Queue Instance::computeQueue = vk::Queue();
vk::Queue Instance::transferQueue = vk::Queue();
std::string Instance::_shaderDirectory = "";
VkDebugReportCallbackEXT Instance::_debugCallback = {};

const uint32_t NVIDIA = 0x10DE;
const uint32_t AMD = 0x1002;
const uint32_t INTEL = 0x8086;

std::vector<const char*> getRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (EnableValidationLayers)
    {
        extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    }

    return extensions;
}

Context* Instance::context() noexcept { return _context.get(); }

std::unique_ptr<UniformBuffer> Instance::getGlobalPropertyBuffer()
{
    return std::make_unique<UniformBuffer>((uint32_t)sizeof(GlobalProperties), 0, Pipeline::globalPropertyLayout());
}

void Instance::raiseValidationError(const std::string& msg)
{
    _log->error(msg);
    _exitCode = 1;
}

std::string Instance::shaderDirectory() noexcept { return _shaderDirectory; }

void Instance::setShaderDirectory(const std::string& path)
{
    if (std::filesystem::exists(path))
    {
        _log->debug("shader directory set to {0}", path);
        _shaderDirectory = path;
    }
    else
    {
        _log->critical("invalid shader directory: " + path);
        throw std::runtime_error("invalid shader directory");
    }
}

void Instance::createInstance()
{
    auto const appInfo = vk::ApplicationInfo()
        .setPApplicationName(APP_NAME)
        .setApplicationVersion(0)
        .setPEngineName(APP_NAME)
        .setApiVersion(VK_API_VERSION_1_0);

    auto const extensions = getRequiredExtensions();

    _log->debug("enabled extensions: {0}", extensions.size());
    for (const auto ex : extensions)
    {
        _log->debug("\t{0}", ex);
    }

    auto info = vk::InstanceCreateInfo()
        .setPApplicationInfo(&appInfo)
        .setEnabledExtensionCount((uint32_t)extensions.size())
        .setPpEnabledExtensionNames(extensions.data());

    if (EnableValidationLayers)
    {
        info.setEnabledLayerCount((uint32_t)validationLayersNames.size());
        info.setPpEnabledLayerNames(validationLayersNames.data());
    }

    _instance = vk::createInstance(info);
    _log->debug("instance created.");
}

void Instance::resizeWindow(GLFWwindow * window, int width, int height)
{
    _log->debug("window 0x{0:x} resized (width: {1}, height: {2})", (std::uintptr_t)window, width, height);

    createContext();
}

void Instance::createSurface(GLFWwindow* window)
{
    _log->debug("surface created");
    check(glfwCreateWindowSurface(_instance, window, nullptr, (VkSurfaceKHR*)&_surface));

    glfwSetFramebufferSizeCallback(window, resizeWindow);
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugReportFlagsEXT flags,
    VkDebugReportObjectTypeEXT objType,
    uint64_t obj, size_t location, int32_t code,
    const char* layerPrefix, const char* msg, void* userData)
{
    auto instance = reinterpret_cast<Instance*>(userData);
    instance->raiseValidationError(msg);
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

void Instance::setupDebugCallbacks()
{
    if (!EnableValidationLayers)
    {
        return;
    }

    VkDebugReportCallbackCreateInfoEXT createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
    createInfo.pfnCallback = debugCallback;

    check(createDebugReportCallbackEXT(_instance, &createInfo, nullptr, &_debugCallback));

    _log->debug("setup debug callback");
}

void Instance::destroyDebugCallbacks()
{
    if (EnableValidationLayers)
    {
        auto vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(
            _instance, "vkDestroyDebugReportCallbackEXT");
        vkDestroyDebugReportCallbackEXT(_instance, _debugCallback, nullptr);

        _log->trace("destroyed debug callbacks");
    }
}

std::string getVendorName(uint32_t vendorID)
{
    switch (vendorID)
    {
    case NVIDIA: return "NVIDIA";
    case AMD: return "AMD";
    case INTEL: return "Intel";
    default: return "Unknown";
    }
}

void getDriverVersion(uint32_t vendorID, uint32_t rawVersion, uint32_t* major, uint32_t *minor, uint32_t *patch)
{
    switch (vendorID)
    {
    case NVIDIA: *major = VK_VERSION_MAJOR(rawVersion); *minor = VK_VERSION_MINOR(rawVersion >> 0) >> 2; *patch = VK_VERSION_PATCH(rawVersion >> 2) >> 4;
        break;
    default: *major = VK_VERSION_MAJOR(rawVersion); *minor = VK_VERSION_MINOR(rawVersion); *patch = VK_VERSION_PATCH(rawVersion);
        break;
    }
}

vk::SurfaceFormatKHR pickSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
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

void Instance::pickPhysicalDevice()
{
    auto gpus = _instance.enumeratePhysicalDevices();

    if (gpus.size() == 0)
    {
        _log->critical("could not locate a Vulkan compatible device.");
        throw std::runtime_error("no device found");
    }

    _log->debug("found {0} device(s)", gpus.size());
    int gpuIndex = 0;
    for (auto const& gpu : gpus)
    {
        auto properties = gpu.getProperties();

        _log->trace("device #{0}", gpuIndex);
        _log->trace("\tID:..............{0}", properties.deviceID);
        _log->trace("\tname:............{0}", properties.deviceName);
        _log->trace("\tvendor:..........{0} ({1})", properties.vendorID, getVendorName(properties.vendorID));
        _log->trace("\ttype:............{0}", vk::to_string(properties.deviceType));

        uint32_t major, minor, patch;
        getDriverVersion(properties.vendorID, properties.driverVersion, &major, &minor, &patch);

        _log->trace("\tdriver version:..{0}.{1}.{2}", major, minor, patch);
        _log->trace("\tVulkan version:..{0}.{1}.{2}", VK_VERSION_MAJOR(properties.apiVersion),
            VK_VERSION_MINOR(properties.apiVersion),
            VK_VERSION_PATCH(properties.apiVersion));

        auto const memoryProperties = gpu.getMemoryProperties();
        vk::DeviceSize vram = 0;
        for (size_t i = 0; i < memoryProperties.memoryHeapCount; i++)
        {
            if (memoryProperties.memoryHeaps[i].flags & vk::MemoryHeapFlagBits::eDeviceLocal)
            {
                vram += memoryProperties.memoryHeaps[i].size;
            }
        }

        _log->trace("\tVRAM:............{0} MB", static_cast<size_t>(vram / 1024 / 1024));

        gpuIndex++;
    }

    if (gpus.size() == 1)
    {
        auto properties = gpus[0].getProperties();
        _log->debug("selected device #0 ({0})", properties.deviceName);
        physicalDevice = gpus[0];
    }
    else
    {
        // TODO : select best physical device
    }
}

void Instance::createDevice()
{
    auto const swapchainExtension = VK_KHR_SWAPCHAIN_EXTENSION_NAME;

    auto const queueFamilies = physicalDevice.getQueueFamilyProperties();
    _log->trace("examining device queues...");

    bool graphicsQueueFound = false;
    bool presentQueueFound = false;
    bool computeQueueFound = false;
    bool transferQueueFound = false;

    uint32_t presentFamily;
    uint32_t graphicsFamily;
    uint32_t computeFamily;
    uint32_t transferFamily;

    int familyIndex = 0;
    for (auto const& queueFamily : queueFamilies)
    {
        _log->trace("queue family #{0}", familyIndex);
        _log->trace("\tqueues:......{0}", queueFamily.queueCount);
        _log->trace("\tflags:.......{0}", vk::to_string(queueFamily.queueFlags));

        if (!graphicsQueueFound && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
        {
            graphicsQueueFound = true;
            _log->trace("selecting family #{0} as graphics queue", familyIndex);
            graphicsFamily = familyIndex;
        }

        if (!computeQueueFound && queueFamily.queueFlags & vk::QueueFlagBits::eCompute)
        {
            computeQueueFound = true;
            _log->trace("selecting family #{0} as compute queue", familyIndex);
            computeFamily = familyIndex;
        }

        if (!transferQueueFound && queueFamily.queueFlags & vk::QueueFlagBits::eTransfer)
        {
            transferQueueFound = true;
            _log->trace("selecting family #{0} as transfer queue", familyIndex);
            transferFamily = familyIndex;
        }

        if (!presentQueueFound && physicalDevice.getSurfaceSupportKHR(familyIndex, _surface) == VK_TRUE)
        {
            presentFamily = familyIndex;
            _log->trace("selecting family #{0} as present queue", familyIndex);
            presentQueueFound = true;
        }

        if (presentQueueFound && graphicsQueueFound && computeQueueFound && transferQueueFound)
        {
            break;
        }

        familyIndex++;
    }

    //std::vector<vk::DeviceQueueCreateInfo> queueInfos;
    std::unordered_map<uint32_t, vk::DeviceQueueCreateInfo> queueMap;

    float priority = 1;

    // graphics queue
    queueMap.insert({ graphicsFamily, vk::DeviceQueueCreateInfo()
        .setQueueCount(1)
        .setQueueFamilyIndex(graphicsFamily)
        .setPQueuePriorities(&priority) });

    // present queue
    queueMap.insert({ presentFamily, vk::DeviceQueueCreateInfo()
        .setQueueCount(1)
        .setQueueFamilyIndex(presentFamily)
        .setPQueuePriorities(&priority) });

    // compute queue
    queueMap.insert({ computeFamily, vk::DeviceQueueCreateInfo()
        .setQueueCount(1)
        .setQueueFamilyIndex(computeFamily)
        .setPQueuePriorities(&priority) });

    // transfer queue
    queueMap.insert({ transferFamily, vk::DeviceQueueCreateInfo()
        .setQueueCount(1)
        .setQueueFamilyIndex(transferFamily)
        .setPQueuePriorities(&priority) });

    auto queueInfos = std::vector<vk::DeviceQueueCreateInfo>();
    for (auto const& queue : queueMap)
    {
        queueInfos.push_back(queue.second);
    }

    auto deviceInfo = vk::DeviceCreateInfo()
        .setEnabledExtensionCount(1)
        .setPpEnabledExtensionNames(&swapchainExtension)
        .setQueueCreateInfoCount((uint32_t)queueInfos.size())
        .setPQueueCreateInfos(queueInfos.data());

    if (EnableValidationLayers)
    {
        deviceInfo.setPpEnabledLayerNames(validationLayersNames.data());
    }

    _log->debug("create device");
    device = physicalDevice.createDevice(deviceInfo);

    graphicsQueue = device.getQueue(graphicsFamily, 0);
    presentQueue = device.getQueue(presentFamily, 0);
    computeQueue = device.getQueue(computeFamily, 0);
    transferQueue = device.getQueue(transferFamily, 0);

    Allocator::init(device, physicalDevice, transferFamily, transferQueue);
}

void Instance::destroyDevice()
{
    _log->trace("destroy device");
    device.destroy();
}

void Instance::createRenderPass()
{
    _log->trace("create render pass");

    // render pass creation
    auto const colorAttachment = vk::AttachmentDescription()
        .setFormat(_surfaceFormat.format)
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
        .setStencilLoadOp(vk::AttachmentLoadOp::eClear)
        .setStencilStoreOp(vk::AttachmentStoreOp::eStore)
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

    std::array<vk::AttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };

    auto const renderPassInfo = vk::RenderPassCreateInfo()
        .setAttachmentCount((uint32_t)attachments.size())
        .setPAttachments(attachments.data())
        .setSubpassCount(1)
        .setPSubpasses(&subpass)
        .setDependencyCount(1)
        .setPDependencies(&dependency);

    renderPass = device.createRenderPass(renderPassInfo);
}

void Instance::initialize(GLFWwindow* window)
{
    _window = window;

    _log = spdlog::stdout_color_mt("instance");
    _log->info("initialize");

#ifdef WIN32
    // this layer causes crashes in the swapchain creation process.
    SetEnvironmentVariable("DISABLE_VK_LAYER_VALVE_steam_overlay_1", "1");
#endif

    createInstance();
    setupDebugCallbacks();
    createSurface(window);
    pickPhysicalDevice();
    createDevice();
    _surfaceFormat = pickSurfaceFormat(physicalDevice.getSurfaceFormatsKHR(_surface));
    createRenderPass();
    Pipeline::initialize();
    createContext();
}

void Instance::createContext()
{
    _context = nullptr;
    int w, h;
    glfwGetFramebufferSize(_window, &w, &h);
    if (w > 0 && h > 0)
    {
        _context = std::make_unique<Context>(_window, _surface, _surfaceFormat, device, physicalDevice);
    }
    else
    {
        _log->trace("not creating context because window has no width or height.");
    }
}

uint32_t Instance::terminate()
{
    _context = nullptr;
    device.destroyRenderPass(renderPass);
    Pipeline::terminate();
    Allocator::terminate();
    destroyDevice();

    destroyDebugCallbacks();
    _instance.destroy();
    _log->trace("destroy vk instance");

    if (_exitCode == 0)
    {
        _log->info("terminate");
    }
    else
    {
        _log->warn("validation error(s) occurred.");
    }

    return _exitCode;
}
