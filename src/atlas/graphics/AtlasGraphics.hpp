#include <vector>
#include <iostream>
#include <memory>
#include <iterator>
#include <set>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "VulkanTools.hpp"
#include "VulkanInitializers.hpp"
#include "SwapchainSupportDetails.hpp"

namespace atlas
{
namespace graphics
{
	class Framebuffer;
	class Renderer;
}
}