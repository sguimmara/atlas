#ifndef ATLAS_DRAWABLE_HPP
#define ATLAS_DRAWABLE_HPP

#include "AtlasGraphics.hpp"
#include "GraphicsObject.hpp"
#include "Renderer.hpp"

namespace atlas
{
    namespace graphics
    {
        /* A drawable is a node that can issue render commands */
        class Drawable : public GraphicsObject
        {
        public:
            Drawable();

            virtual void prepareRenderState() = 0;
            virtual void tick(float dt) {}
            virtual void UpdateCommandBuffers() = 0;
            virtual vk::CommandBuffer GetCommandBuffer(uint32_t imageIndex) = 0;

        protected:
            std::vector<vk::CommandBuffer> _commandBuffers;
            Renderer* _renderer;
        };
    }
}

#endif