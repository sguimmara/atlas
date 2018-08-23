#include "GraphicsObject.hpp"

namespace atlas
{
    namespace graphics
    {
        GraphicsObject::GraphicsObject() :
            mLocalTransform(1.0f) // identity matrix
        {
        }

        GraphicsObject::~GraphicsObject()
        {
        }

        void GraphicsObject::setTransform(Transform t)
        {
            mLocalTransform = t;
        }

        void GraphicsObject::prepareRenderState()
        {
        }
    }
}