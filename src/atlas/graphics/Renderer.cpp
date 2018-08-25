#include "Renderer.hpp"

#include "Mesh.hpp"

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

using namespace atlas::graphics;

Renderer::Renderer()
{
    mLog = spdlog::stdout_color_mt("renderer");

#if DEBUG
    mLog->set_level(spdlog::level::trace);
#endif

    mLog->info("started");
}

Renderer::~Renderer()
{
    if (enableValidationLayers)
    {
        auto vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(
            mVkInstance, "vkDestroyDebugReportCallbackEXT");
        vkDestroyDebugReportCallbackEXT(mVkInstance, mVkValidationCallback, nullptr);
    }

    delete mDrawable;

    destroySemaphores();
    destroyFramebuffer();
    destroyCommandPool();
    destroyLogicalDevice();
    destroyInstance();

    mLog->debug("destroyed");
}

void Renderer::init(GLFWwindow * window)
{
    mWindow = window;

    createInstance();
    setupDebugCallback();
    createSurface(window);
    selectPhysicalDevice();
    createLogicalDevice();
    createSemaphores();
    createCommandPool();
    createFramebuffer();

    //TEMP
    mClearColor = { 1, 0, 0, 1 };
    auto mesh = new Mesh(this, nullptr, nullptr);
    mDrawable = mesh;
    
}

void Renderer::onWindowResized(int newWidth, int newHeight)
{
    mWindowResized = true;
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

/* creates the Vulkan instance, checking for supported layers and extensions */
void Renderer::createInstance()
{
    if (enableValidationLayers && !checkValidationLayerSupport())
    {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    if (enableValidationLayers)
    {
        mLog->debug("validation layers are enabled");
    }

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = APP_NAME;
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    appInfo.pEngineName = APP_NAME;
    appInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    mLog->debug("query Vulkan extensions ({0})", createInfo.enabledExtensionCount);
    for (const auto ext : extensions)
    {
        mLog->debug("extension: {0}", ext);
    }

    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    mLog->debug("query Vulkan layers ({0})", createInfo.enabledLayerCount);
    for (const auto vl : validationLayers)
    {
        mLog->debug("layer: {0}", vl);
    }

    VK_CHECK_RESULT(vkCreateInstance(&createInfo, nullptr, &mVkInstance));

    mLog->debug("Vulkan instance created");
}

void Renderer::destroyInstance()
{
    vkDestroyInstance(mVkInstance, nullptr);
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

void Renderer::setupDebugCallback()
{
    if (!enableValidationLayers) return;

    VkDebugReportCallbackCreateInfoEXT createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
    createInfo.pfnCallback = debugCallback;

    VK_CHECK_RESULT(createDebugReportCallbackEXT(mVkInstance, &createInfo, nullptr, &mVkValidationCallback));

    mLog->debug("setup debug callback");
}

void Renderer::createSurface(GLFWwindow* window)
{
    VK_CHECK_RESULT(glfwCreateWindowSurface(mVkInstance, window, nullptr, &mVkSurface));
    mLog->debug("surface created");
}

QueueFamilyIndices Renderer::findQueueFamilies(VkPhysicalDevice device)
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    mLog->debug("found {0:d} queue families", queueFamilies.size());

    int i = 0;
    for (const auto& queueFamily : queueFamilies)
    {
        if (!indices.isComplete())
        {
            if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.graphicsFamily = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, mVkSurface, &presentSupport);

            if (queueFamily.queueCount > 0 && presentSupport)
            {
                indices.presentFamily = i;
            }
        }

        i++;
    }

    return indices;
}

bool checkDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions)
    {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

bool Renderer::isDeviceSuitable(VkPhysicalDevice device, QueueFamilyIndices indices)
{
    bool extensionsSupported = checkDeviceExtensionSupport(device);

    bool swapChainAdequate = false;
    if (extensionsSupported)
    {
        SwapChainSupportDetails swapChainSupport = SwapChainSupportDetails::querySwapChainSupport(device, mVkSurface);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

bool Renderer::checkSupportedImageFormat(VkFormat format)
{
    VkImageFormatProperties fmtProperties;
    if (vkGetPhysicalDeviceImageFormatProperties(
        mVkPhysicalDevice,
        format,
        VK_IMAGE_TYPE_2D,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        0,
        &fmtProperties) == VK_ERROR_FORMAT_NOT_SUPPORTED)
    {
        return false;
    }

    return true;
}

/* select an appropriate physical device, eg GPU */
void Renderer::selectPhysicalDevice()
{
    uint32_t deviceCount = 0;
    VK_CHECK_RESULT(vkEnumeratePhysicalDevices(mVkInstance, &deviceCount, nullptr));

    if (deviceCount == 0)
    {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    VK_CHECK_RESULT(vkEnumeratePhysicalDevices(mVkInstance, &deviceCount, devices.data()));

    for (const auto& device : devices)
    {
        QueueFamilyIndices indices = findQueueFamilies(device);
        if (isDeviceSuitable(device, indices))
        {
            mFamilyIndices = indices;
            mVkPhysicalDevice = device;
            break;
        }
    }

    if (mVkPhysicalDevice == VK_NULL_HANDLE)
    {
        throw std::runtime_error("failed to find a suitable GPU!");
    }

    if (!checkSupportedImageFormat(VK_FORMAT_R8G8B8A8_UNORM))
    {
        throw std::runtime_error("fatal: image format not supported: VK_FORMAT_R8G8B8A8_UNORM");
    }

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(mVkPhysicalDevice, &deviceProperties);
    mLog->info("physical device selected : {0}", deviceProperties.deviceName);
    mLog->info("driver version {0}", deviceProperties.driverVersion);

    int apiVer = deviceProperties.apiVersion;
    mLog->info("Vulkan API version {0}.{1}.{2}",
        VK_VERSION_MAJOR(apiVer),
        VK_VERSION_MINOR(apiVer),
        VK_VERSION_PATCH(apiVer));
}

void Renderer::createLogicalDevice()
{
    QueueFamilyIndices indices = mFamilyIndices;

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<int> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };

    float queuePriority = 1.0f;
    for (int queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures = {};

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(mVkPhysicalDevice, &createInfo, nullptr, &mVkDevice) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(mVkDevice, indices.graphicsFamily, 0, &mVkGraphicsQueue);
    mLog->debug("retrieved graphics queue");
    vkGetDeviceQueue(mVkDevice, indices.presentFamily, 0, &mVkPresentQueue);
    mLog->debug("retrieved present queue");
}

void Renderer::destroyLogicalDevice()
{
    vkDestroyDevice(mVkDevice, nullptr);
    mLog->debug("destroyed device");
}

void Renderer::createSemaphores()
{
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VK_CHECK_RESULT(vkCreateSemaphore(mVkDevice, &semaphoreInfo, nullptr, &mSemaphoreRenderFinished));
    VK_CHECK_RESULT(vkCreateSemaphore(mVkDevice, &semaphoreInfo, nullptr, &mSemaphoreImageAvailable));
}

void Renderer::destroySemaphores()
{
    vkDestroySemaphore(mVkDevice, mSemaphoreRenderFinished, nullptr);
    vkDestroySemaphore(mVkDevice, mSemaphoreImageAvailable, nullptr);
}

void Renderer::createCommandPool()
{
    VkCommandPoolCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    info.queueFamilyIndex = mFamilyIndices.graphicsFamily;
    info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VK_CHECK_RESULT(vkCreateCommandPool(mVkDevice, &info, nullptr, &mVkCommandPool));

    mLog->debug("created command pool");
}

void Renderer::destroyCommandPool()
{
    vkDestroyCommandPool(mVkDevice, mVkCommandPool, nullptr);
    mLog->debug("destroyed command pool");
}

void Renderer::createFramebuffer()
{
    mFramebuffer = new Framebuffer(mWindow, mVkDevice, mVkPhysicalDevice, mVkSurface, mFamilyIndices.graphicsFamily, mFamilyIndices.presentFamily);
}

void Renderer::destroyFramebuffer()
{
    assert(mFramebuffer);
    delete mFramebuffer;
    mFramebuffer = nullptr;
    mLog->debug("destroyed framebuffer");
}

void Renderer::mainLoop()
{
    mLog->debug("enter main loop");

    while (!glfwWindowShouldClose(mWindow))
    {
        auto start = std::chrono::steady_clock::now();

        glfwPollEvents();
        checkFramebufferResized();
        renderFrame();
        vkDeviceWaitIdle(mVkDevice);
        vkQueueWaitIdle(mVkGraphicsQueue);
        vkQueueWaitIdle(mVkPresentQueue);

        //getc(stdin);

        auto end = std::chrono::steady_clock::now();

        mFrameDuration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    }

    vkDeviceWaitIdle(mVkDevice);

    mLog->debug("exit main loop");
}

void Renderer::checkFramebufferResized()
{
    if (mWindowResized)
    {
        mLog->debug("framebuffer resized");
        mWindowResized = false;

        vkDeviceWaitIdle(mVkDevice);
        destroyFramebuffer();
        createFramebuffer();

        //scene->prepareRenderState();
        mDrawable->prepareRenderState();
    }
}

void Renderer::renderFrame()
{
    currentFrame++;

    uint32_t imageIndex = mFramebuffer->acquire(mSemaphoreImageAvailable);

    submitCommands(imageIndex);

    mFramebuffer->present(imageIndex, mSemaphoreRenderFinished);
}

void Renderer::submitCommands(uint32_t imageIndex)
{
    VkSemaphore waitSemaphores[] = { mSemaphoreImageAvailable };
    VkSemaphore signalSemaphores[] = { mSemaphoreRenderFinished };

    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    std::vector<VkCommandBuffer> commands;

    //for (Drawable * drawable : getRenderList(*scene))
    //{
    //    commands.push_back(drawable->getCommandBuffer(imageIndex));
    //}

    //commands.push_back(mDrawable->getCommandBuffer(imageIndex));

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = (uint32_t)commands.size();
    submitInfo.pCommandBuffers = commands.data();

    VK_CHECK_RESULT(vkQueueSubmit(mVkGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE));
}

uint32_t Renderer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(mVkPhysicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    throw std::runtime_error("could not find appropriate memory type");
}
