#ifndef ATLAS_BLANK_HPP
#define ATLAS_BLANK_HPP

#include "AtlasGraphics.hpp"

#include "Drawable.hpp"

namespace atlas
{
    namespace graphics
    {
        class Blank : public Drawable
        {
            virtual void prepareRenderState() override;
            virtual vk::CommandBuffer GetCommandBuffer(uint32_t imageIndex) override;
            virtual void UpdateCommandBuffers() override;
        };
    }
}

#endif