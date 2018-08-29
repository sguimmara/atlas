#ifndef ATLAS_NODE_HPP
#define ATLAS_NODE_HPP

#include "AtlasGraphics.hpp"

namespace atlas
{
    namespace graphics
    {

        /**
        * @brief      Base class for all graphical elements
        */
        class GraphicsObject
        {
        public:
            GraphicsObject();
            ~GraphicsObject();

            void setTransform(Transform t);
            inline Transform localTransform() const noexcept { return _localTransform; }
            inline Transform modelView() const noexcept { return _modelView; }
            virtual void prepareRenderState();

        protected:
            Transform _localTransform;
            Transform _modelView;
        };
    }
}

#endif