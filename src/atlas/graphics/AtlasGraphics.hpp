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

#ifndef GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_LEFT_HANDED
#endif
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
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

const double PI = 3.141592653589793238463;
const float  PI_F = 3.14159265358979f;

#define CHECK_SUCCESS(x) (assert(x == vk::Result::eSuccess))

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

#endif
