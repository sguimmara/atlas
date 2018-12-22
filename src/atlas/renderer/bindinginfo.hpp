#pragma once
#ifndef ATLAS_RENDERER_BINDINGINFO_HPP
#define ATLAS_RENDERER_BINDINGINFO_HPP

#include <functional>
#include <string>
#include "common.hpp"

namespace atlas::renderer
{
    struct BindingInfo
    {
        std::string name;
        uint32_t index;
        uint32_t set;
        vk::DescriptorType type;

        BindingInfo() {}

        BindingInfo(std::string name, uint32_t index, uint32_t set, vk::DescriptorType type) :
            name(name), index(index), set(set), type(type)
        {}

        BindingInfo(const BindingInfo& rhs) :
            name(rhs.name), index(rhs.index), set(rhs.set), type(rhs.type)
        {}

        bool operator==(const BindingInfo& rhs) const noexcept
        {
            return name == rhs.name
                && index == rhs.index
                && set == rhs.set
                && type == rhs.type;
        }

        bool operator!=(const BindingInfo& rhs) const noexcept
        {
            return !(*this == rhs);
        }

        static bool compare(const BindingInfo& lhs, const BindingInfo& rhs)
        {
            return (lhs == rhs);
        }
    };
}

namespace std
{
   template<>
   struct hash<atlas::renderer::BindingInfo>
   {
       size_t operator()(const atlas::renderer::BindingInfo& ub) const
       {
           auto const strHash = std::hash<std::string>();
           return ub.index ^ ub.set ^ (uint32_t)ub.type ^ strHash(ub.name);
       }
   };
}

#endif