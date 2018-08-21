#include "atlas/graphics/Renderer.hpp"

#include "spdlog/sinks/stdout_color_sinks.h"

#include "VulkanTools.hpp"

#ifdef DEBUG
const bool enableValidationLayers = true;
#else
const bool enableValidationLayers = false;
#endif

const std::vector<const char*> validationLayers =
{
    "VK_LAYER_LUNARG_standard_validation"
};

atlas::graphics::Renderer::Renderer()
{
    mLog = spdlog::stdout_color_mt("renderer");

#if DEBUG
    mLog->set_level(spdlog::level::trace);
#endif

    mLog->info("started");
}

void atlas::graphics::Renderer::init(GLFWwindow * window)
{
    create_instance();
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
void atlas::graphics::Renderer::create_instance()
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

    VK_CHECK_RESULT(vkCreateInstance(&createInfo, nullptr, &instance));

    mLog->debug("Vulkan instance created");
}
