#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

typedef glm::mat4 Transform;

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