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
            vk::SurfaceCapabilitiesKHR capabilities;
            std::vector<vk::SurfaceFormatKHR> formats;
            std::vector<vk::PresentModeKHR> presentModes;

            static SwapChainSupportDetails QuerySwapChainSupport(vk::PhysicalDevice device, vk::SurfaceKHR surface)
            {
                SwapChainSupportDetails details;

                device.getSurfaceCapabilitiesKHR(surface, &details.capabilities);

                uint32_t formatCount;
                device.getSurfaceFormatsKHR(surface, &formatCount, nullptr);

                if (formatCount != 0)
                {
                    details.formats.resize(formatCount);
                    device.getSurfaceFormatsKHR(surface, &formatCount, details.formats.data());
                }

                uint32_t presentModeCount;
                device.getSurfacePresentModesKHR(surface, &presentModeCount, nullptr);

                if (presentModeCount != 0)
                {
                    details.presentModes.resize(presentModeCount);
                    device.getSurfacePresentModesKHR(surface, &presentModeCount, details.presentModes.data());
                }

                return details;
            }
        };
    }
}

#endif // ATLAS_VULKAN_SWAPCHAINSUPPORTDETAILS_HPP