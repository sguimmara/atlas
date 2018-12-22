#pragma once
#ifndef ATLAS_RENDERER_COMMON_HPP
#define ATLAS_RENDERER_COMMON_HPP

#include <vulkan/vulkan.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <glm/glm.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

using namespace glm;

inline void check(vk::Result result)
{
    if (result != vk::Result::eSuccess)
    {
        throw std::runtime_error("unexpected Vk result : " + vk::to_string(result));
    }
}

inline void check(VkResult result)
{
    return check((vk::Result)result);
}

#endif // ATLAS_RENDERER_COMMON_HPP