#ifndef ATLAS_GRAPHICS_HPP
#define ATLAS_GRAPHICS_HPP

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
#include <vulkan/vulkan.hpp>

#include "atlas/core/Color.hpp"

#include "VulkanTools.hpp"
#include "VulkanInitializers.hpp"
#include "SwapchainSupportDetails.hpp"

#ifndef DEBUG
#define VERIFY(x) assert(x)
#else
#define VERIFY(x) ((void)(x))
#endif

#define ERR_EXIT(err_msg, err_class) \
    do {                             \
        _log->critical(err_msg);     \
        _log->flush();               \
        std::terminate();            \
    } while (0)

namespace atlas
{
namespace graphics
{
    class Framebuffer2;
    class Renderer2;
}
}

#endif