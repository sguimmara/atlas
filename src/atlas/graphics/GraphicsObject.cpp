#include "GraphicsObject.hpp"

namespace atlas
{
    namespace graphics
    {
        GraphicsObject::GraphicsObject() :
            _localTransform(1.0f),
            _modelView(1.0)
        {
        }

        GraphicsObject::~GraphicsObject()
        {
        }

        void GraphicsObject::setTransform(Transform t)
        {
            _localTransform = t;
        }

        void GraphicsObject::prepareRenderState()
        {
        }
    }
}