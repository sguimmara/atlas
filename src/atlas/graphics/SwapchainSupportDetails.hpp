#ifndef ATLAS_SWAPCHAINSUPPORTDETAILS_HPP
#define ATLAS_SWAPCHAINSUPPORTDETAILS_HPP

#include "AtlasGraphics.hpp"

namespace atlas
{
    namespace graphics
    {
        /**
        * @brief      Encapsulate swapchain capabilities of the current platform
        */
        struct SwapChainSupportDetails
        {
            VkSurfaceCapabilitiesKHR capabilities;
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentModes;

            static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
            {
                SwapChainSupportDetails details;

                vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

                uint32_t formatCount;
                vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

                if (formatCount != 0)
                {
                    details.formats.resize(formatCount);
                    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
                }

                uint32_t presentModeCount;
                vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

                if (presentModeCount != 0)
                {
                    details.presentModes.resize(presentModeCount);
                    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
                }

                return details;
            }
        };
    }
}

#endif // ATLAS_VULKAN_SWAPCHAINSUPPORTDETAILS_HPP