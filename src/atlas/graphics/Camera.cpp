#include "Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace atlas
{
    namespace graphics
    {
        Camera::Camera(Renderer* renderer) :
            _fovRadians(1),
            _aspect(1.333f),
            _viewport(renderer->viewport()),
            _projectionMatrix(glm::perspective<float>(_fovRadians, _aspect, 0.05f, 15.0f)),
            Node()
        {
            _localTransform = glm::lookAt(glm::vec3(0, 0, -10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        }
    }
}