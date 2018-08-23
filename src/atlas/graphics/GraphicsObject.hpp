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
            inline Transform localTransform() const noexcept { return mLocalTransform; }
            inline Transform modelView() const noexcept { return mModelView; }
            virtual void prepareRenderState();

        protected:
            Transform mLocalTransform;
            Transform mModelView;
        };
    }
}

#endif